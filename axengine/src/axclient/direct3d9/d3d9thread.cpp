/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "d3d9private.h"

AX_BEGIN_NAMESPACE

QueuedScene *d3d9Scene;

D3D9Window *d3d9FrameWnd;
bool d3d9IsReflecting;
bool d3d9ForceWireframe = false;

QueuedScene *d3d9WorldScene;
D3D9Target *d3d9WorldTarget;
const QueuedEntity *d3d9Actor;
Interaction *d3d9Interaction;

RenderTarget *d3d9BoundTarget = 0;


static D3D9Target *s_gbuffer;
static D3D9Target *s_lbuffer;	// light buffer


static Technique s_technique;

static TexFormat sColorFormat = TexFormat::BGRA8;
static TexFormat sDepthFormat = TexFormat::D24S8;

static bool s_threadRendering;


static Matrix4 GetConvertMatrix()
{
	Matrix4 mat;
	mat.setIdentity();
	mat.scale(1, 1, 0.5);
	mat.translate(0, 0, 0.5);
	return mat;
}

static void ConvertToD3D(Matrix4 &m) {
	static Matrix4 conv = GetConvertMatrix();

	Matrix4 m2 = conv * m;
	m = m2;
}



void D3D9clearer::doClear() const
{
	DWORD d3dclear = 0;

	if (isClearDepth) {
		d3dclear |= D3DCLEAR_ZBUFFER;
		if (d3d9StateManager->getDepthStencilFormat().isStencil()) {
			d3dclear |= D3DCLEAR_STENCIL;
		}
	}

	if (isClearStencil) {
		d3dclear |= D3DCLEAR_STENCIL;
	}

	if (isClearColor) {
		d3dclear |= D3DCLEAR_TARGET;
	}

	if (!d3dclear) {
		return;
	}

	D3DCOLOR d3dcolor = D3DCOLOR_RGBA(color.r,color.g,color.b,color.a);

	d3d9Device->Clear(0, 0, d3dclear, d3dcolor, depth, stencil);
}


D3D9Thread::D3D9Thread()
{
	m_frameId = 0;
}

D3D9Thread::~D3D9Thread()
{}

void D3D9Thread::runFrame(bool isInThread)
{
	double startTime = OsUtil::seconds();

	d3d9Queue = g_queues[m_frameId%2];
	m_frameId++;

	if (isInThread) {
		d3d9Queue->beginConsuming();
		d3d9Mutex.lock();
	}

	s_threadRendering = isInThread;

	beginFrame();
	d3d9Device->BeginScene();

	D3D9Window *window = dynamic_cast<D3D9Window*>(d3d9Queue->getTarget());
	if (window != d3d9FrameWnd) {
		d3d9FrameWnd = window;
		d3d9FrameWnd->bind();
	}

	double cachestart = OsUtil::seconds();

	syncFrame();

	double cacheend = OsUtil::seconds();

	int view_count = d3d9Queue->getSceneCount();
	float frametime = d3d9Queue->getScene(0)->camera.getFrameTime();

	D3D9clearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true);

	if (r_nulldraw.getBool()) {
//			goto endframe;
	}


	float scenetime[16];
	m_isStatistic = false;

	for (int i = 0; i < view_count; i++) {
		if (i == view_count - 1) m_isStatistic = true;

		double s = OsUtil::seconds();
		QueuedScene *queued = d3d9Queue->getScene(i);

		drawScene(queued, clearer);
		clearer.clearColor(false);
		clearer.clearDepth(false);

		scenetime[i] = OsUtil::seconds() - s;
	}


	d3d9Device->EndScene();
	endFrame();

	double end = OsUtil::seconds();

	if (frametime <= 0) {
		frametime = 1;
	}

	float backendtime = end - startTime;

#if 0
	g_statistic->setValue(stat_fps, 1.0f / frametime);
	g_statistic->setValue(stat_frameTime, frametime * 1000);
	g_statistic->setValue(stat_cacheTime, (cacheend-cachestart)*1000);
	g_statistic->setValue(stat_backendTime, backendtime*1000);
#else
	stat_fps.setInt(1.0f / frametime);
	stat_frameTime.setInt(frametime * 1000);
	stat_cacheTime.setInt((cacheend-cachestart)*1000);
	stat_backendTime.setInt(backendtime*1000);
#endif
	if (isInThread) {
		d3d9Mutex.unlock();
		d3d9Queue->endConsuming();
	}
}

void D3D9Thread::beginFrame()
{
	if (!r_specular.getBool()) {
		g_shaderMacro.setMacro(ShaderMacro::G_DISABLE_SPECULAR);
	} else {
		g_shaderMacro.resetMacro(ShaderMacro::G_DISABLE_SPECULAR);
	}
}

void D3D9Thread::drawScene(QueuedScene *scene, const D3D9clearer &clearer)
{
	if (scene->sceneType == QueuedScene::WorldMain) {
		drawScene_world(scene, clearer);
	} else if (scene->sceneType == QueuedScene::Default) {
		drawScene_noworld(scene, clearer);
	} else {
		Errorf("D3D9Thread::drawScene: error scene");
	}
}

void D3D9Thread::setupScene(QueuedScene *scene, const D3D9clearer *clearer, RenderTarget *target, RenderCamera *camera)
{
	//		AX_ASSERT(scene);
	if (!scene && !camera) {
		Errorf("D3D9Thread::setupScene: parameter error");
		return;
	}

	if (!camera) {
		camera = &scene->camera;
	}

	d3d9Camera = camera;

	if (!target) {
		target = camera->getTarget();
	}

	bindTarget(target);

	// clear here, befor viewport and scissor set, so clear all rendertarget's area,
	// if you want clear only viewport and scissor part, call clear after this function
	if (clearer) {
		clearer->doClear();
	}

	const Vector4 &vp = camera->getViewPortDX();
	D3DVIEWPORT9 d3dviewport;
	d3dviewport.X = vp.x;
	d3dviewport.Y = vp.y;
	d3dviewport.Width = vp.z;
	d3dviewport.Height = vp.w;
	d3dviewport.MinZ = 0;
	d3dviewport.MaxZ = 1;
	RECT d3dRect;
	d3dRect.left = vp.x;
	d3dRect.top = vp.y;
	d3dRect.right = vp.x + vp.z;
	d3dRect.bottom = vp.y + vp.w;
	d3d9Device->SetViewport(&d3dviewport);
	d3d9Device->SetScissorRect(&d3dRect);

	AX_SU(g_time, (float)camera->getTime());

	Vector4 campos = camera->getOrigin();
	if (camera->isOrthoProjection()) {
		campos.w = 0;
	} else {
		campos.w = 1;
	}
	AX_SU(g_cameraPos, campos);

	Angles angles = camera->getViewAxis().toAngles();
	angles *= AX_D2R;

	AX_SU(g_cameraAngles, angles.toVector3());
	AX_SU(g_cameraAxis, camera->getViewAxis().getTranspose());

	Matrix4 temp = camera->getViewProjMatrix();
	ConvertToD3D(temp);
	AX_SU(g_viewProjMatrix, temp);
	temp = camera->getViewProjNoTranslate();
	ConvertToD3D(temp);
	AX_SU(g_viewProjNoTranslate, temp);

	d3d9Interaction = nullptr;
	d3d9Actor = nullptr;

	AX_SU(g_modelMatrix, Matrix::getIdentity());
	AX_SU(g_instanceParam, Vector4(0,0,0,1));

	Rect r = target->getRect();
	AX_SU(g_sceneSize, Vector4(r.width,r.height,1.0f/r.width, 1.0f/r.height));

	if (camera->isReflectionEnabled()) {
		d3d9IsReflecting = true;
	} else {
		d3d9IsReflecting = false;
	}
}

void D3D9Thread::unsetScene(QueuedScene *scene, const D3D9clearer *clearer, RenderTarget *target, RenderCamera *camera)
{
	if (!scene && !camera) {
		Errorf("D3D9Thread::unsetScene: parameter error");
		return;
	}

	if (!camera) {
		camera = &scene->camera;
	}

	if (!target) {
		target = camera->getTarget();
	}
}

void D3D9Thread::drawPrimitive(int prim_id)
{
	d3d9Interaction = nullptr;

	D3D9Primitive *prim = d3d9PrimitiveManager->getPrimitive(prim_id);

	if (!prim) {
		return;
	}

	// check actor
	if (d3d9Actor) {
		d3d9Actor = nullptr;
		AX_SU(g_modelMatrix, Matrix::getIdentity());
	}
	prim->draw(Technique::Main);
}

void D3D9Thread::drawInteraction(Interaction *ia)
{
	static bool primMatrixSet = false;
	d3d9Interaction = ia;

	D3D9Primitive *prim = d3d9PrimitiveManager->getPrimitive(ia->resource);

	if (!prim) {
		return;
	}

	// check actor
	const QueuedEntity *re = ia->queuedEntity;
	if (re != d3d9Actor || prim->isMatrixSet() || primMatrixSet) {
		d3d9Actor = re;

		if (d3d9Actor) {
			if (prim->isMatrixSet()) {
				primMatrixSet = true;
			} else {
				primMatrixSet = false;
			}

			AX_SU(g_modelMatrix, d3d9Actor->matrix);
			AX_SU(g_instanceParam, d3d9Actor->instanceParam);

			if (prim->isMatrixSet()) {
				Matrix mat = prim->getMatrix().getAffineMat();
				mat = d3d9Actor->matrix * mat;
				AX_SU(g_modelMatrix, d3d9Actor->matrix);
			}

			if (d3d9Actor->flags & RenderEntity::DepthHack) {
//					glDepthRange(0, 0.3f);
			} else {
//					glDepthRange(0, 1);
			}
		} else {
			AX_SU(g_modelMatrix, Matrix::getIdentity());
			AX_SU(g_instanceParam, Vector4(0,0,0,1));
		}
	}
	prim->draw(s_technique);
}


void D3D9Thread::endFrame()
{
	d3d9FrameWnd->present();

	d3d9Queue->clear();
}

void D3D9Thread::syncFrame()
{
	BEGIN_PIX("SyncFrame");
	Material::syncFrame();
	D3D9Texture::syncFrame();
	d3d9TargetManager->syncFrame();
	d3d9QueryManager->syncFrame();
	d3d9PrimitiveManager->syncFrame();

	d3d9VertexBufferManager->reportStatices();

	d3d9VertexBufferManager->beginAlloc();

	int viewcount = d3d9Queue->getSceneCount();

	for (int i = 0; i < viewcount; i++) {
		QueuedScene *queued = d3d9Queue->getScene(i);

		cacheSceneRes(queued);

		for (int j = 0; j < queued->numSubScenes; j++) {
			cacheSceneRes(queued->subScenes[j]);
		}
	}

	if (1)
		d3d9Queue->endSync();

	d3d9VertexBufferManager->endAlloc();
	END_PIX();
}

void D3D9Thread::cacheSceneRes(QueuedScene *scene)
{
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

		scene->interactions[j]->resource = d3d9PrimitiveManager->cachePrimitive(prim);

		if (scene->sceneType != QueuedScene::WorldMain)
			continue;

		if (scene->numDebugInteractions >= QueuedScene::MAX_DEBUG_INTERACTIONS)
			continue;

		if (normallen > 0.00001f) {
			if (prim->getType() == Primitive::MeshType) {
				if (r_ignorMesh.getBool()) {
					scene->interactions[j]->resource = -1;
				}
				MeshPrim *mesh = dynamic_cast<MeshPrim*>(prim);
				if (mesh == nullptr)
					continue;
				LinePrim *line = mesh->getNormalLine(normallen);
				Interaction *ia = d3d9Queue->allocInteraction();
				ia->queuedEntity = scene->interactions[j]->queuedEntity;
				ia->primitive = line;
//					ia->resource = d3d9PrimitiveManager->cachePrimitive(line);

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
			Interaction *ia = d3d9Queue->allocInteraction();
			ia->queuedEntity = scene->interactions[j]->queuedEntity;
			ia->primitive = line;
//				ia->resource = d3d9PrimitiveManager->cachePrimitive(line);

			scene->debugInteractions[scene->numDebugInteractions++] = ia;
		}
	}

	if (!s_view) {
		return;
	}

	for (int i=0; i<scene->numDebugInteractions; i++) {
		scene->debugInteractions[i]->resource = d3d9PrimitiveManager->cachePrimitive(scene->debugInteractions[i]->primitive);
	}

	scene->numPrimitives = s2i(s_view->primitives.size());
	scene->primIds = d3d9Queue->allocPrimitives(scene->numPrimitives);

	for (int j = 0; j < scene->numPrimitives; j++) {
		scene->primIds[j] = d3d9PrimitiveManager->cachePrimitive(s_view->primitives[j]);
	}

	scene->numOverlayPrimitives = s2i(s_view->overlays.size());
	scene->overlayPrimIds = d3d9Queue->allocPrimitives(scene->numOverlayPrimitives);

	for (int j = 0; j < scene->numOverlayPrimitives; j++) {
		scene->overlayPrimIds[j] = d3d9PrimitiveManager->cachePrimitive(s_view->overlays[j]);
	}
}

void D3D9Driver::runFrame()
{
	d3d9Thread->runFrame(false);
}

void D3D9Thread::drawPass_zfill(QueuedScene *scene)
{
	s_technique = Technique::Zpass;

	D3D9clearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true, Rgba::Zero);

//		d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	setupScene(scene, &clearer, s_gbuffer);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	unsetScene(scene, &clearer, s_gbuffer);
//		d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
}

void D3D9Thread::drawPass_composite(QueuedScene *scene)
{

	if (r_wireframe.getBool()) {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		d3d9ForceWireframe = true;
	} else {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		d3d9ForceWireframe = false;
	}

	s_technique = Technique::Main;

	setupScene(scene, 0, scene->target);

	issueVisQuery();

#if 1
	if (!r_showLightBuf.getBool()) {
		D3D9clearer clearer;
		clearer.clearDepth(false);
		clearer.clearColor(true, scene->clearColor);
		clearer.doClear();

		for (int i = 0; i < scene->numInteractions; i++) {
			drawInteraction(scene->interactions[i]);
		}
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primIds[i]);
	}
#endif
	unsetScene(scene, 0, scene->target);

	d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	d3d9ForceWireframe = false;
}

inline DWORD F2DW(FLOAT f)
{
	return *((DWORD*)&f);
}

void D3D9Thread::drawPass_shadowGen(QueuedScene *scene)
{
	if (!scene->numInteractions) {
		return;
	}

	D3D9Target *target = (D3D9Target*)scene->camera.getTarget();
	if (target->isPooled() && !target->alreadyAllocatedRealTarget() )
		return;

	D3D9Texture *tex = (D3D9Texture*)scene->camera.getTarget()->getTexture();

	QueuedLight *qlight = scene->sourceLight;
	QueuedShadow *qshadow = qlight->shadowInfo;

	if (r_shadowGen.getBool()) {
		BEGIN_PIX("ShadowGen");
		// offset the geometry slightly to prevent z-fighting
		// note that this introduces some light-leakage artifacts
		float factor = gl_shadowOffsetFactor.getFloat();
		float units = gl_shadowOffsetUnits.getFloat() / 0x10000;

		d3d9StateManager->SetRenderState(D3DRS_DEPTHBIAS, F2DW(units));
		d3d9StateManager->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(factor));

		s_technique = Technique::ShadowGen;

		D3D9clearer clearer;
		clearer.clearDepth(true);

		setupScene(scene, 0);
		clearer.doClear();

		d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		for (int i = 0; i < scene->numInteractions; i++) {
			drawInteraction(scene->interactions[i]);
		}

		d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);

		if (scene->isLastCsmSplits()) {
			issueShadowQuery();
		}

		unsetScene(scene);

		// disable depth biase
		d3d9StateManager->SetRenderState(D3DRS_DEPTHBIAS, 0);
		d3d9StateManager->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
		END_PIX();
	}

	tex->setHardwareShadowMap(true);
	tex->setFilterMode(Texture::FM_Linear);
	tex->setClampMode(Texture::CM_Clamp);
	tex->setBorderColor(Rgba::White);

	scene->rendered = true;
}

void D3D9Thread::drawPass_lights(QueuedScene *scene)
{
	D3D9clearer clearer;

	clearer.clearColor(true, Rgba::Zero);

	if (r_showLightBuf.getBool()) {
		s_lbuffer = 0;
	}

	setupScene(d3d9WorldScene, &clearer, s_lbuffer);
	clearer.clearColor(false);

	for (int i = 0; i < scene->numLights; i++) {
		QueuedLight *ql = scene->lights[i];

		if (ql->type == RenderLight::kGlobal) {
			drawGlobalLight(scene, ql);
		} else {
			drawLocalLight(scene, ql);
		}
	}

	unsetScene(d3d9WorldScene, nullptr, s_lbuffer);
}

void D3D9Thread::drawGlobalLight( QueuedScene *scene, QueuedLight *light )
{
	d3d9Postprocess->drawGlobalLight(light->lightVolume, light);
}

void D3D9Thread::drawLocalLight(QueuedScene *scene, QueuedLight *light)
{
	QueuedShadow *qshadow = light->shadowInfo;

	if (qshadow) {
		for (int i=0; i<qshadow->numSplitCamera; i++) {
			// mask stencil
			d3d9StateManager->SetRenderState(D3DRS_STENCILENABLE, TRUE);
			d3d9Postprocess->maskVolumeTwoSides(qshadow->splitVolumes[i]);

			// draw to light buffer
			d3d9Postprocess->drawLightShadowed(qshadow->splitVolumes[i], light, qshadow->splitCameras[i]);

			d3d9StateManager->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		}
	} else {
		// mask stencil
//		d3d9StateManager->SetRenderState(D3DRS_STENCILENABLE, TRUE);
//		d3d9Postprocess->maskVolume(light->lightVolume);

		// draw to light buffer
		d3d9Postprocess->drawLight(light->lightVolume, light);

//		d3d9StateManager->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
}

void D3D9Thread::drawPass_postprocess(QueuedScene *scene)
{}

void D3D9Thread::drawPass_overlay(QueuedScene *scene)
{
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

void D3D9Thread::drawScene_world(QueuedScene *scene, const D3D9clearer &clearer)
{
	BEGIN_PIX("DrawWorld");

	s_technique = Technique::Main;
	d3d9WorldScene = scene;

	const Rect &rect = scene->camera.getViewRect();
	int width = rect.width;
	int height = rect.height;

	if (1) {
		s_gbuffer = d3d9FrameWnd->m_gbuffer;
		AX_SU(g_sceneDepth, s_gbuffer->getTextureDX());

		s_lbuffer = d3d9FrameWnd->m_lightBuffer;
		AX_SU(g_lightBuffer, s_lbuffer->getTextureDX());

	} else {
		d3d9WorldTarget = 0;
	}

	// set exposure
	float exposure = scene->exposure;
	if (exposure == 0) {
		g_shaderMacro.resetMacro(ShaderMacro::G_HDR);
		exposure = 1;
	} else {
		g_shaderMacro.setMacro(ShaderMacro::G_HDR);
	}

#if 0
	g_statistic->setValue(stat_exposure, exposure * 100);
#else
	stat_exposure.setInt(exposure * 100);
#endif
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

	// draw subscene first
	for (int i = 0; i < scene->numSubScenes; i++) {
		QueuedScene *sub = scene->subScenes[i];

		if (sub->sceneType == QueuedScene::ShadowGen) {
			drawPass_shadowGen(sub);
		} else if (sub->sceneType == QueuedScene::Reflection) {
			BEGIN_PIX("ReflectionGen");
			g_shaderMacro.setMacro(ShaderMacro::G_REFLECTION);
			drawScene_worldSub(sub);
			g_shaderMacro.resetMacro(ShaderMacro::G_REFLECTION);
			END_PIX();
		} else if (sub->sceneType == QueuedScene::RenderToTexture) {
			BEGIN_PIX("RenderToTexture");
			drawScene_worldSub(sub);
			END_PIX();
		}
	}

	// fill z first
	BEGIN_PIX("GfillPass");
	drawPass_zfill(scene);
	END_PIX();

	BEGIN_PIX("DrawLights");
	drawPass_lights(scene);
	END_PIX();

	BEGIN_PIX("SceneComposite");
	drawPass_composite(scene);
	END_PIX();

	// post process and render back to backbuffer
	if (r_framebuffer.getBool()) {
		BEGIN_PIX("PostProcess");
		drawPass_postprocess(scene);
		END_PIX();
	}

	// check if need draw overlay primitives
	BEGIN_PIX("DrawOverlay");
	drawPass_overlay(scene);
	END_PIX();

	END_PIX();
}

void D3D9Thread::drawScene_worldSub(QueuedScene *scene)
{
	s_technique = Technique::Main;

	D3D9clearer clear;
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

void D3D9Thread::drawScene_noworld(QueuedScene *scene, const D3D9clearer &clearer)
{
	BEGIN_PIX("DrawNoworld");
	s_technique = Technique::Main;

	setupScene(scene, &clearer, scene->target);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	double start = OsUtil::seconds();
	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primIds[i]);
	}
	double end = OsUtil::seconds();

	unsetScene(scene, nullptr, scene->target);

	drawPass_overlay(scene);

	if (m_isStatistic)
#if 0
		g_statistic->setValue(stat_staticsTime, (end - start) * 1000);
#else
		stat_staticsTime.setInt((end - start) * 1000);
#endif
	END_PIX();
}

void D3D9Thread::bindTarget(RenderTarget *target)
{
	if (d3d9BoundTarget != target || target->isWindow()) {
		if (d3d9BoundTarget) {
			d3d9BoundTarget->unbind();
		}
		target->bind();
		d3d9BoundTarget = target;
	}
}

void D3D9Thread::issueVisQuery()
{
	d3d9Postprocess->issueQueryList(d3d9QueryManager->getActiveQuery(Query::QueryType_Vis));
}

void D3D9Thread::issueShadowQuery()
{
	const List<D3D9querymanager::ActiveQuery*>& querylist = d3d9QueryManager->getActiveQuery(Query::QueryType_Shadow);
	int count = 0;

	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	List<D3D9querymanager::ActiveQuery*>::const_iterator it = querylist.begin();
	for (; it != querylist.end(); ++it) {
		D3D9querymanager::ActiveQuery *active = *it;

		// have issued
		if (active->issued)
			continue;

		count++;
		D3D9Query *query = active->query;
		if (!query->beginQuery())
			continue;

		d3d9Postprocess->issueBboxQuery(active->bbox);

		query->endQuery();
		active->issued = true;
	}
	d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
#if 0
	g_statistic->setValue(stat_numShadowQuery, count);
#else
	stat_numShadowQuery.setInt(count);
#endif
}

void D3D9Thread::doRun()
{
	while (1) {
//			d3d9Queue->beginConsuming();

		runFrame(true);

//			d3d9Queue->endConsuming();
	}
}

AX_END_NAMESPACE
