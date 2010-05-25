/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE

RenderCamera *gCamera;
QueuedScene *gScene;

GLwindow *gFrameWindow;
bool gIsReflecting;

QueuedScene *gWorldScene;
GLtarget *gWorldTarget;
GLframebuffer *gWorldFramebuffer;

const QueuedEntity *gActor;

static Technique s_technique;
static bool s_clearShadowmask;

static GLtarget *s_gbuffer;
static bool s_shadowGened;

static int s_numShadowMap;
static GLtexture *s_shadowMap[16];

static TexFormat sColorFormat = TexFormat::BGRA8;
static TexFormat sDepthFormat = TexFormat::D24S8;

static inline void BindTarget(RenderTarget *target) {
	static RenderTarget *bound;

	if (bound != target) {
		if (target->isTexture()) {
			target->bind();
		} else if (bound && bound->isTexture()) {
			bound->unbind();
		}

		bound = target;
	}
}


void Clearer::doClear() const {
	GLbitfield clearbits = 0;

	if (isClearDepth) {
		clearbits |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(depth);
	}

	if (isClearStencil || isClearDepth) {
		clearbits |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(stencil);
	}

	if (isClearColor) {
		clearbits |= GL_COLOR_BUFFER_BIT;
		Vector4 colorf = color.toVector4();
		glClearColor(colorf.x, colorf.y, colorf.z, colorf.w);
	}

	if (!clearbits) {
		return;
	}

	glClear(clearbits);
}

GLthread::GLthread()
	: m_threadRendering(false)
{
}

GLthread::~GLthread() {}


void GLthread::initialize() {
#if 0
	for (int i = 0; i < Queue::QUERY_FRAME_STACK_DEPTH; i++) {
		glGenQueries(Queue::MAX_QUERIES, m_queryIds[i]);
	}
	TypeZeroArray(m_numQueries);

	m_queryFrameNum = 0;
	m_curQueryBucket = 0;
	m_curResultBucket = Queue::QUERY_FRAME_STACK_DEPTH - 1;
	m_queryOffset = 0;
	m_readOffset = 1;
#endif
}

void GLthread::finalize() {
}

void GLthread::runFrame(bool isInThread) {
	beginFrame();

	m_threadRendering = isInThread;

	GLwindow *window = dynamic_cast<GLwindow*>(g_renderQueue->getTarget());
#if 0
	m_numQueries[m_curQueryBucket] = gRenderQueue->allocQueryId();
#endif
	if (window != gFrameWindow) {
		gFrameWindow = window;
		gFrameWindow->bind();
	}

	double cachestart = OsUtil::seconds();

	cacheResource();

	double cacheend = OsUtil::seconds();

	int view_count = g_renderQueue->getSceneCount();
	float frametime = g_renderQueue->getScene(0)->camera.getFrameTime();

	Clearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true);

	if (r_nulldraw.getBool()) {
//			goto endframe;
	}

	for (int i = 0; i < view_count; i++) {
		QueuedScene *queued = g_renderQueue->getScene(i);

		drawScene(queued, clearer);
		clearer.clearColor(false);
	}

	endFrame();

	double end = OsUtil::seconds();

	if (frametime <= 0) {
		frametime = 1;
	}

	int backendtime = end - cachestart;

#if 0
	g_statistic->setValue(stat_fps, 1000 / frametime);
	g_statistic->setValue(stat_frameTime, frametime);
	g_statistic->setValue(stat_cacheTime, cacheend-cachestart);
	g_statistic->setValue(stat_frontendTime, frametime - backendtime);
	g_statistic->setValue(stat_backendTime, backendtime);
#else
	stat_fps.setInt(1000 / frametime);
	stat_frameTime.setInt(frametime);
	stat_cacheTime.setInt(cacheend-cachestart);
	stat_frontendTime.setInt(frametime - backendtime);
	stat_backendTime.setInt(backendtime);
#endif
}

void GLthread::doRun() {
//		initialize();

	while (1) {
		g_renderQueue->beginConsuming();

		runFrame(true);

		g_renderQueue->endConsuming();
	}

//		finalize();
}

void GLthread::beginFrame() {
	glPrimitiveManager->beginFrame();

	if (!r_specular.getBool()) {
		g_shaderMacro.setMacro(ShaderMacro::G_DISABLE_SPECULAR);
	} else {
		g_shaderMacro.resetMacro(ShaderMacro::G_DISABLE_SPECULAR);
	}
}

void GLthread::drawScene(QueuedScene *scene, const Clearer &clearer) {
	if (scene->sceneType == QueuedScene::WorldMain) {
		drawScene_world(scene, clearer);
	} else if (scene->sceneType == QueuedScene::Default) {
		drawScene_noworld(scene, clearer);
	} else {
		Errorf("GLthread::drawScene: error scene");
	}
}

static inline float sgn(float a) {
	if (a > 0.0F) return (1.0F);
	if (a < 0.0F) return (-1.0F);
	return (0.0F);
}

static inline float Dot(Vector4 v1, Vector4 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

static void ModifyProjectionMatrix(const Vector4 &clipPlane, float matrix[16])
{
//		float       matrix[16];
	Vector4    q;

	// Grab the current projection matrix from OpenGL
//		glGetFloatv(GL_PROJECTION_MATRIX, matrix);

	// Calculate the clip-space corner point opposite the clipping plane
	// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
	// transform it into camera space by multiplying it
	// by the inverse of the projection matrix

	q.x = (sgn(clipPlane.x) + matrix[8]) / matrix[0];
	q.y = (sgn(clipPlane.y) + matrix[9]) / matrix[5];
	q.z = -1.0F;
	q.w = (1.0F + matrix[10]) / matrix[14];

	// Calculate the scaled plane vector
	Vector4 c = clipPlane * (2.0F / Dot(clipPlane, q));

	// Replace the third row of the projection matrix
	matrix[2] = c.x;
	matrix[6] = c.y;
	matrix[10] = c.z + 1.0F;
	matrix[14] = c.w;

	// Load it back into OpenGL
//		glMatrixMode(GL_PROJECTION);
//		glLoadMatrixf(matrix);
}

void GLthread::setupScene(QueuedScene *scene, const Clearer *clearer, RenderTarget *target, RenderCamera *camera) {
//		AX_ASSERT(scene);
	if (!scene && !camera) {
		Errorf("GLthread::setupScene: parameter error");
		return;
	}

	if (!camera) {
		camera = &scene->camera;
	}

	gCamera = camera;

	if (!target) {
		target = camera->getTarget();
	}

#if 0
	if (target->isTexture()) {
		target->bind();
	}
#else
	BindTarget(target);
#endif
	const Vector4 &viewport = camera->getViewPort();
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
	glScissor(viewport.x, viewport.y, viewport.z, viewport.w);

	AX_SU(g_time, camera->getTime());
	AX_SU(g_cameraPos, camera->getOrigin());

	Angles angles = camera->getViewAxis().toAngles();
	angles *= AX_D2R;

	AX_SU(g_cameraAngles, angles.toVector3());
	AX_SU(g_cameraAxis, camera->getViewAxis());

	AX_SU(g_viewProjMatrix, camera->getViewProjMatrix());
	AX_SU(g_viewProjNoTranslate, camera->getViewProjNoTranslate());

	gCurInteraction = nullptr;
	gActor = nullptr;

	AX_SU(g_modelMatrix, Matrix::getIdentity());
	AX_SU(g_instanceParam, Vector4(0,0,0,1));

	Rect r = target->getRect();
	AX_SU(g_sceneSize, Vector4(r.width,r.height,1.0f/r.width, 1.0f/r.height));

	if (camera->isReflectionEnabled()) {
		gIsReflecting = true;
		glFrontFace(GL_CW);
	} else {
		gIsReflecting = false;
		glFrontFace(GL_CCW);
	}

	if (clearer) {
		clearer->doClear();
	}
}

void GLthread::unsetScene(QueuedScene *scene, const Clearer *clearer, RenderTarget *target, RenderCamera *camera) {
	if (!scene && !camera) {
		Errorf("GLthread::unsetScene: parameter error");
	}

	if (!camera) {
		camera = &scene->camera;
	}

	if (!target) {
		target = camera->getTarget();
	}
#if 0
	if (target->isTexture()) {
		target->unbind();
	}
#endif
}

void GLthread::drawPrimitive(int prim_id) {
	gCurInteraction = nullptr;

	GLprimitive *prim = glPrimitiveManager->getPrimitive(prim_id);

	if (!prim) {
		return;
	}

	// check actor
	if (gActor) {
		gActor = nullptr;
		glDepthRange(0, 1);
		AX_SU(g_modelMatrix, Matrix::getIdentity());
	}
	prim->draw(Technique::Main);
}

void GLthread::drawInteraction(Interaction *ia) {
	static bool primMatrixSet = false;
	gCurInteraction = ia;

	GLprimitive *prim = glPrimitiveManager->getPrimitive(ia->resource);

	if (!prim) {
		return;
	}

	// check actor
	const QueuedEntity *re = ia->queuedEntity;
	if (re != gActor || prim->isMatrixSet() || primMatrixSet) {
		gActor = re;

		if (gActor) {
			if (prim->isMatrixSet()) {
				primMatrixSet = true;
			} else {
				primMatrixSet = false;
			}

			AX_SU(g_modelMatrix, gActor->matrix);
			AX_SU(g_instanceParam, gActor->instanceParam);

			if (prim->isMatrixSet()) {
				Matrix mat = prim->getMatrix().getAffineMat();
				mat = gActor->matrix * mat;
				AX_SU(g_modelMatrix, gActor->matrix);
			}

			if (gActor->flags & RenderEntity::DepthHack) {
				glDepthRange(0, 0.3f);
			} else {
				glDepthRange(0, 1);
			}
		} else {
			glDepthRange(0, 1);
			AX_SU(g_modelMatrix, Matrix::getIdentity());
			AX_SU(g_instanceParam, Vector4(0,0,0,1));
		}
	}
	prim->draw(s_technique);
}


void GLthread::endFrame() {
	gFrameWindow->swapBuffers();

	GLrender::checkErrors();

	glPrimitiveManager->endFrame();

	g_renderQueue->clear();
}

void GLthread::cacheResource() {
	int viewcount = g_renderQueue->getSceneCount();

	for (int i = 0; i < viewcount; i++) {
		QueuedScene *queued = g_renderQueue->getScene(i);

		cacheSceneRes(queued);

		for (int j = 0; j < queued->numSubScenes; j++) {
			cacheSceneRes(queued->subScenes[j]);
		}
	}

	if (m_threadRendering)
		g_renderQueue->endSync();
}

void GLthread::cacheSceneRes(QueuedScene *scene) {
	RenderScene *s_view = scene->source;

//		scene->camera = s_view->camera;

	float tangentlen = r_showTangents.getFloat();
	float normallen = r_showNormal.getFloat();

	for (int j = 0; j < scene->numInteractions; j++) {
		Primitive *prim = scene->interactions[j]->primitive;

		if (prim->getType() == Primitive::MeshType) {
			if (r_ignorMesh.getBool()) {
				scene->interactions[j]->resource = -1;
				continue;
			}
		}

		scene->interactions[j]->resource = glPrimitiveManager->cachePrimitive(scene->interactions[j]->primitive);

		if (scene->numDebugInteractions == QueuedScene::MAX_DEBUG_INTERACTIONS)
			break;

		if (normallen > 0.00001f) {
			if (prim->getType() == Primitive::MeshType) {
				if (r_ignorMesh.getBool()) {
					scene->interactions[j]->resource = -1;
				}
				MeshPrim *mesh = dynamic_cast<MeshPrim*>(prim);
				if (mesh == nullptr)
					continue;
				LinePrim *line = mesh->getNormalLine(normallen);
				Interaction *ia = g_renderQueue->allocInteraction();
				ia->queuedEntity = scene->interactions[j]->queuedEntity;
				ia->primitive = line;
				ia->resource = glPrimitiveManager->cachePrimitive(line);

				scene->debugInteractions[scene->numDebugInteractions++] = ia;
			}

			continue;
		}

		// check if need draw tangent space
		if (tangentlen < 0.00001f)
			continue;

		if (prim->getType() == Primitive::MeshType) {
			if (r_ignorMesh.getBool()) {
				scene->interactions[j]->resource = -1;
			}
			MeshPrim *mesh = dynamic_cast<MeshPrim*>(prim);
			if (mesh == nullptr)
				continue;
			LinePrim *line = mesh->getTangentLine(tangentlen);
			Interaction *ia = g_renderQueue->allocInteraction();
			ia->queuedEntity = scene->interactions[j]->queuedEntity;
			ia->primitive = line;
			ia->resource = glPrimitiveManager->cachePrimitive(line);

			scene->debugInteractions[scene->numDebugInteractions++] = ia;
		}
	}

	if (!s_view) {
		return;
	}

	scene->numPrimitives = s2i(s_view->primitives.size());
	scene->primIds = g_renderQueue->allocPrimitives(scene->numPrimitives);

	for (int j = 0; j < scene->numPrimitives; j++) {
		scene->primIds[j] = glPrimitiveManager->cachePrimitive(s_view->primitives[j]);
	}

	scene->numOverlayPrimitives = s2i(s_view->overlays.size());
	scene->overlayPrimIds = g_renderQueue->allocPrimitives(scene->numOverlayPrimitives);

	for (int j = 0; j < scene->numOverlayPrimitives; j++) {
		scene->overlayPrimIds[j] = glPrimitiveManager->cachePrimitive(s_view->overlays[j]);
	}
}

void GLthread::preFrame() {
	glFramebufferManager->preFrame();

	// get last frame's query result
#if 0
	for (int i = 0; i < m_numQueries[m_curResultBucket]; i++) {
		glGetQueryObjectiv(m_queryIds[m_curResultBucket][i], GL_QUERY_RESULT_ARB, &queryResult[i]);
	}
	gRenderQueue->setQueryResult(m_numQueries[m_curResultBucket], queryResult);

	gRenderQueue->m_readOffset = m_readOffset;
	QueryResults::iterator it = gRenderQueue->m_queryResults[m_readOffset].begin();
	for (; it != gRenderQueue->m_queryResults[m_readOffset].end(); ++it) {
		glGetQueryObjectiv(it->first + m_readOffset, GL_QUERY_RESULT_ARB, &it->second);
	}
#endif
	GLrender::checkErrors();

}

void GLdriver::runFrame() {
	glThread->runFrame(false);
}

void GLthread::drawPass_zfill(QueuedScene *scene) {
	s_technique = Technique::Zpass;

	Clearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true, scene->clearColor);

	GLrender::checkErrors();
	setupScene(scene, &clearer, s_gbuffer);

//		glColorMask(0, 0, 0, 0);
//		glStencilMask(0);
	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}
//		glColorMask(1, 1, 1, 1);

	AX_SU(g_sceneDepth, s_gbuffer->getTextureGL());

	unsetScene(scene, &clearer, s_gbuffer);
}

void GLthread::drawPass_composite(QueuedScene *scene) {

	if (r_wireframe.getBool()) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	s_technique = Technique::Main;

	setupScene(scene, nullptr, scene->target);
#if 1
	Clearer clearer;
	clearer.clearDepth(false);
	clearer.clearColor(true, scene->clearColor);
	clearer.doClear();

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primIds[i]);
	}
#endif
	unsetScene(scene, nullptr, scene->target);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GLthread::drawPass_shadowGen(QueuedScene *scene) {
	if (!scene->numInteractions) {
		return;
	}

	QueuedLight *qlight = scene->sourceLight;
	QueuedShadow *qshadow = qlight->shadowInfo;

	GLtexture *tex = (GLtexture*)scene->camera.getTarget()->getTexture();
	if (r_shadowGen.getBool()) {
		// offset the geometry slightly to prevent z-fighting
		// note that this introduces some light-leakage artifacts
		float factor = gl_shadowOffsetFactor.getFloat();
		float units = gl_shadowOffsetUnits.getFloat();

		if (tex->getFormat().getDepthBits() == 24) {
			units *= 256;
		}
		glPolygonOffset(factor, units);
		glEnable(GL_POLYGON_OFFSET_FILL);

		s_technique = Technique::ShadowGen;

		Clearer clearer;
		clearer.clearDepth(true);
		if (/*qshadow->useCsmAtlas && */s_shadowGened) {
			clearer.clearDepth(false);
		}

		setupScene(scene, &clearer);

		glColorMask(0, 0, 0, 0);

		for (int i = 0; i < scene->numInteractions; i++) {
			drawInteraction(scene->interactions[i]);
		}

		glColorMask(1, 1, 1, 1);

		unsetScene(scene);

		// is if shadowgen, render to shadow mask
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	tex->setHardwareShadowMap(true);
	tex->setFilterMode(Texture::FM_Linear);
	tex->setClampMode(Texture::CM_ClampToEdge);
	tex->setBorderColor(Rgba::White);

	s_shadowMap[s_numShadowMap++] = tex;

	scene->rendered = true;
	s_shadowGened = true;
}

#if 0
void GLthread::drawPass_shadowMasks(QueuedScene *scene) {
	for (int i = 0; i < scene->numLights; i++) {
		QueuedLight *ql = scene->lights[i];
		QueuedShadow *qshadow = ql->shadowInfo;

		if (!qshadow) {
			continue;
		}

		if (1/*qshadow->useCsmAtlas*/) {
			Matrix4 matrix = qshadow->splitCameras[0].getViewProjMatrix();
			matrix.scale(0.5f, 0.5f, 0.5f);
			matrix.translate(0.5f, 0.5f, 0.5f);

			drawPass_shadowMask(qshadow->shadowGenScenes[0], matrix);
			continue;
		}

		for (int j = 0; j < qshadow->numSplitCamera; j++) {
			Matrix4 matrix = qshadow->splitCameras[j].getViewProjMatrix();
			matrix.scale(0.5f, 0.5f, 0.5f);
			matrix.translate(0.5f, 0.5f, 0.5f);

			drawPass_shadowMask(qshadow->shadowGenScenes[j], matrix);
		}
	}
}


void GLthread::drawPass_shadowMask(QueuedScene *scene, const Matrix4 &matrix) {
#if 0
	QueuedLight *light = scene->sourceLight;
	QueuedShadow *qshadow = light->shadowInfo;

	if (1/*qshadow->useCsmAtlas*/) {
		if (!s_shadowGened) {
			return;
		}
	} else {
		if (!scene->rendered) {
			return;
		}
	}

	GLtexture *tex = (GLtexture*)scene->camera.getTarget()->getTexture();

	s_technique = Technique::Main;

	// draw shadow map to shadow mask
	Clearer clearer;

	if (s_clearShadowmask) {
		s_clearShadowmask = false;
		clearer.clearColor(true, Rgba::White);
	}

	if (!r_shadowMask->getBool()) {
		return;
	}

	int splitIndex = scene->splitIndex;

//		Vector2 db = qshadow->splitDepthBounds[splitIndex];
	QueuedLight::VolumeVertexes *vv = &qshadow->splitVolumes[splitIndex];
	
	if (1/*qshadow->useCsmAtlas*/) {
		vv = &qshadow->splitAllVolume;
//			db.y = qshadow->splitDepthBounds[qshadow->numSplitCamera-1].y;
	}

	setupScene(gWorldScene, nullptr, gShadowMaskTarget);

	if (SUPPORT_GL_EXT_depth_bounds_test && r_useDepthBoundsExt->getBool()) {
		clearer.doClear();
		glEnable(GL_DEPTH_BOUNDS_TEST_EXT);
#if 0
		glDepthBoundsEXT(scene->shadowBounds.x, scene->shadowBounds.y);
#else
//			glDepthBoundsEXT(db.x, db.y);
#endif
	} else {
		clearer.clearStencil(true, 127);

		// mask stencil
		glEnable(GL_STENCIL_TEST);

		glStencilMask(255);
		clearer.doClear();
		glColorMask(0, 0, 0, 0);

		glStencilFunc(GL_ALWAYS, 0, 255);

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);

		glPostprocess->maskVolume(*vv);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_NOTEQUAL, 127, 255 );

		glStencilMask(0);
		glColorMask(1, 1, 1, 1);
	}

	// draw shadow mask
	bool drawfront = r_useMaskVolumeFront->getBool();
	drawfront = false;

	if (qshadow->useCsmAtlas) {
//			glPostprocess->maskShadow(*vv, matrix, tex, drawfront);
		Matrix4 &mat = *(Matrix4*)qshadow->splitScaleOffsets;
		glPostprocess->maskShadow(*vv, matrix, tex, qshadow->splitMinrange, qshadow->splitMaxrange, mat, drawfront);
	} else {
		glPostprocess->maskShadow(*vv, matrix, tex, drawfront);
	}

	if (SUPPORT_GL_EXT_depth_bounds_test && r_useDepthBoundsExt->getBool()) {
		glDisable(GL_DEPTH_BOUNDS_TEST_EXT);
	} else {
		glDisable(GL_STENCIL_TEST);
	}

	unsetScene(gWorldScene, nullptr, gShadowMaskTarget);
#endif
}

void GLthread::drawPass_shadowBlur(QueuedScene *scene) {
	int blurtimes = r_shadowBlur->getInteger();

	if (blurtimes <= 0) {
		return;
	}

	blurtimes = blurtimes * 2;

	GLtarget *src = gShadowMaskTarget;
	GLtarget *dst = src->getFramebuffer()->allocTarget(RenderTarget::FrameAlloc, sColorFormat);
	dst->attachDepth(src->getDepthAttached());

	// draw overlay
	RenderCamera camera = scene->camera;
	camera.setOverlay(camera.getViewRect());
	s_technique = Technique::Main;

	for (int i = 0; i < blurtimes; i++) {
		setupScene(scene, nullptr, dst, &camera);
		glPostprocess->shadowBlur(src->getTextureGL(), !(i%2));
		unsetScene(scene, nullptr, dst, &camera);

		std::swap(dst, src);
	}
}
#endif

void GLthread::drawPass_postprocess(QueuedScene *scene) {
	// draw overlay
	GLtarget *downscale = 0;
	GLtarget *downscale2 = 0;

	if (r_bloom.getBool()) {
		const Rect &r = scene->camera.getViewRect();
		int width = r.width / 4;
		int height = r.height / 4;

		downscale = (GLtarget*)glFramebufferManager->allocTarget(RenderTarget::PermanentAlloc, width, height, sColorFormat);
		downscale2 = (GLtarget*)glFramebufferManager->allocTarget(RenderTarget::PermanentAlloc, width, height, sColorFormat);

		glPostprocess->genericPP("_downscale4x4bright", downscale, gWorldTarget->getTextureGL());
		glPostprocess->genericPP("_gaussblurh", downscale2, downscale->getTextureGL());
		glPostprocess->genericPP("_gaussblurv", downscale, downscale2->getTextureGL());
	}

	RenderCamera camera = scene->camera;
	camera.setOverlay(camera.getViewRect());

	GLtexture *tex = (GLtexture*)gWorldTarget->getTexture();

	if (r_showShadowMap.getInteger()) {
		int id = r_showShadowMap.getInteger() - 1;
		if (id >= 0 && id < s_numShadowMap) {
			tex = s_shadowMap[id];
			tex->setHardwareShadowMap(false);
		}
	}

	Clearer clear;
	clear.clearColor(true);
	setupScene(scene, 0, nullptr, &camera);
#if 1
	if (scene->m_histogramQueryId) {
		beginQuery(scene->m_histogramQueryId);
		glPostprocess->measureHistogram(tex, scene->m_histogramIndex);
		endQuery();
	}

	if (r_bloom.getBool()) {
		downscale->getTextureGL()->setFilterMode(Texture::FM_Linear);
		glPostprocess->genericPP("_combine2", gWorldTarget->getTextureGL(), downscale->getTextureGL());
//			glPostprocess->drawQuad(downscale->getTextureGL());
	} else {
		gWorldTarget->getTextureGL()->setFilterMode(Texture::FM_Nearest);
		glPostprocess->drawQuad(gWorldTarget->getTextureGL());
	}
#endif
	unsetScene(scene, nullptr, nullptr, &camera);
}

void GLthread::drawPass_overlay(QueuedScene *scene) {
	if (!scene->numOverlayPrimitives) {
		return;
	}

	// draw overlay
	RenderCamera camera = scene->camera;
	camera.setOverlay(camera.getViewRect());

	setupScene(scene, nullptr, nullptr, &camera);

	for (int i = 0; i < scene->numOverlayPrimitives; i++) {
		drawPrimitive(scene->overlayPrimIds[i]);
	}

	unsetScene(scene, nullptr, nullptr, &camera);
}

void GLthread::drawScene_world(QueuedScene *scene, const Clearer &clearer) {

	double start = OsUtil::seconds();

	s_technique = Technique::Main;
	gWorldScene = scene;

	s_numShadowMap = 0;
	s_shadowGened = false;

	const Vector4 &viewport = scene->camera.getViewPort();

	gWorldFramebuffer = glFramebufferManager->getFramebuffer(viewport.z, viewport.w);

	if (r_framebuffer.getBool()) {
		GLtarget *colortarget = gWorldFramebuffer->allocTarget(RenderTarget::PermanentAlloc, sColorFormat);
		GLtarget *depth = gWorldFramebuffer->allocTarget(RenderTarget::PermanentAlloc, sDepthFormat);
		GLtarget *gbuffer = gWorldFramebuffer->allocTarget(RenderTarget::PermanentAlloc, TexFormat::RGBA16F);
		gbuffer->getTexture()->setFilterMode(Texture::FM_Nearest);
		gbuffer->getTexture()->setClampMode(Texture::CM_ClampToEdge);
		colortarget->attachDepth(depth);
		gbuffer->attachDepth(depth);

		s_gbuffer = gbuffer;

		scene->target = colortarget;
		gWorldTarget = colortarget;
	} else {
		gWorldTarget = 0;
	}

#if 0
	// set g_zrecoverParam
	float n = scene->camera.getZnear();
	float f = scene->camera.getZfar();
//		Vector4 zrecoverParam(n, f, f * n, f - n);
	Vector4 zrecoverParam(n, f, -(f-n)/(f*n), (1/n));
	AX_SU(g_zrecoverParam, zrecoverParam);
#endif
	// set exposure
	float exposure = scene->exposure;
	if (exposure == 0) {
		g_shaderMacro.resetMacro(ShaderMacro::G_HDR);
		exposure = 1;
	} else {
		g_shaderMacro.setMacro(ShaderMacro::G_HDR);
	}

	//g_statistic->setValue(stat_exposure, exposure * 100);
	stat_exposure.setInt(exposure * 100);
	AX_SU(g_exposure, Vector4(1.0f/exposure, exposure,0,0));

	// set global light parameter
	if (scene->globalLight) {
		AX_SU(g_globalLightPos, scene->globalLight->pos);
		AX_SU(g_globalLightColor, scene->globalLight->color);
		AX_SU(g_skyColor, scene->globalLight->skyColor);
	}

	// set global fog
	if (scene->globalFog && r_fog.getBool()) {
		g_shaderMacro.setMacro(ShaderMacro::G_FOG);
		AX_SU(g_fogParams, scene->globalFog->m_fogParams);
	} else {
		g_shaderMacro.resetMacro(ShaderMacro::G_FOG);
	}

	if (scene->waterFog && r_fog.getBool()) {
		AX_SU(g_waterFogParams, scene->waterFog->m_fogParams);
	}

	AX_SU(g_windMatrices, scene->windMatrices);
	AX_SU(g_leafAngles, scene->leafAngles);

#if 1
	// draw subscene first
	for (int i = 0; i < scene->numSubScenes; i++) {
		QueuedScene *sub = scene->subScenes[i];

		if (sub->sceneType == QueuedScene::ShadowGen) {
			drawPass_shadowGen(sub);
		} else if (sub->sceneType == QueuedScene::Reflection) {
			g_shaderMacro.setMacro(ShaderMacro::G_REFLECTION);
			drawScene_worldSub(sub);
			g_shaderMacro.resetMacro(ShaderMacro::G_REFLECTION);
		} else if (sub->sceneType == QueuedScene::RenderToTexture) {
			drawScene_worldSub(sub);
		}
	}

	GLrender::checkErrors();
	// fill z first
	drawPass_zfill(scene);

#if 0
	drawPass_shadowMasks(scene);

	drawPass_shadowBlur(scene);
#endif
#endif
	drawPass_composite(scene);

	// post process and render back to backbuffer
	if (r_framebuffer.getBool()) {
		drawPass_postprocess(scene);
	}

	// check if need draw overlay primitives
	drawPass_overlay(scene);

	double end = OsUtil::seconds();
//		Printf("FRAMETIME: %d", end - start);
}

void GLthread::drawScene_worldSub(QueuedScene *scene) {
	s_technique = Technique::Main;

	Clearer clear;
	clear.clearDepth(true);
	clear.clearColor(true, scene->clearColor);

	// no shadow, no light, no fog etc. direct composite
	setupScene(scene, &clear);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primIds[i]);
	}

	unsetScene(scene, &clear);
}

void GLthread::drawScene_noworld(QueuedScene *scene, const Clearer &clearer) {
	s_technique = Technique::Main;

	glColorMask(1, 1, 1, 1);

	setupScene(scene, &clearer, scene->target);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primIds[i]);
	}

	unsetScene(scene, nullptr, scene->target);

	drawPass_overlay(scene);
}

void GLthread::beginQuery(int id) {
}

void GLthread::endQuery() {
}

AX_END_NAMESPACE

