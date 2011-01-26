#include "../private.h"

AX_BEGIN_NAMESPACE

MeshUP::MeshUP()
{
	m_numVertices = 0;
	m_vertices = nullptr;
	m_numIndices = 0;
	m_indices = nullptr;
}

MeshUP::~MeshUP()
{
	TypeFree(m_vertices);
	TypeFree(m_indices);
}

void MeshUP::init(int num_vertices, int num_indices)
{
	if (m_numVertices || m_numIndices) {
		Errorf("mesh already initilized");
	}

	m_numVertices = num_vertices;
	m_vertices = TypeAlloc<Vector3>(m_numVertices);
	m_numIndices = num_indices;
	m_indices = TypeAlloc<ushort_t>(m_numIndices);
}

bool MeshUP::setupScreenQuad(MeshUP*& mesh, const Rect &rect)
{
	bool result = false;

	if (!mesh) {
		mesh = new MeshUP();
		mesh->init(4, 6);
		mesh->m_indices[0] = 0;
		mesh->m_indices[1] = 1;
		mesh->m_indices[2] = 2;
		mesh->m_indices[3] = 2;
		mesh->m_indices[4] = 1;
		mesh->m_indices[5] = 3;
	}

	mesh->m_vertices[0].set(rect.x, rect.y, 0.0f);
	mesh->m_vertices[1].set(rect.x + rect.width, rect.y, 0.0f);
	mesh->m_vertices[2].set(rect.x, rect.y + rect.height, 0.0f);
	mesh->m_vertices[3].set(rect.x + rect.width, rect.y + rect.height, 0.0f);

	return result;
}

bool MeshUP::setupHexahedron(MeshUP*& mesh, Vector3 vertes[8])
{
	bool result = false;
	int numverts = 8;
	int numindexes = 6 * 2 * 3;

	if (!mesh) {
		result = true;
		mesh = new MeshUP();
		mesh->init(numverts, numindexes);

		// triangles
		static ushort_t s_idxes[] = {
			0, 2, 1, 1, 2, 3,
			2, 6, 3, 3, 6, 7,
			6, 4, 7, 7, 4, 5,
			4, 0, 5, 5, 0, 1,
			1, 3, 5, 5, 3, 7,
			0, 4, 2, 2, 4, 6
		};

		memcpy(mesh->m_indices, s_idxes, numindexes * sizeof(ushort_t));
	}

	memcpy(mesh->m_vertices, vertes, 8 * sizeof(Vector3));

	return result;
}

bool MeshUP::setupBoundingBox( MeshUP*& mesh, const BoundingBox &bbox )
{
	bool result = false;
	int numverts = 8;
	int numindexes = 6 * 2 * 3;

	if (!mesh) {
		result = true;
		mesh = new MeshUP();
		mesh->init(numverts, numindexes);

		// triangles
		static ushort_t s_idxes[] = {
			0, 2, 1, 1, 2, 3,
			2, 6, 3, 3, 6, 7,
			6, 4, 7, 7, 4, 5,
			4, 0, 5, 5, 0, 1,
			1, 3, 5, 5, 3, 7,
			0, 4, 2, 2, 4, 6
		};

		memcpy(mesh->m_indices, s_idxes, numindexes * sizeof(ushort_t));
	}

	Vector3 *verts = mesh->m_vertices;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				verts[i*4+j*2+k].x = i == 0 ? bbox.min.x : bbox.max.x; 
				verts[i*4+j*2+k].y = j == 0 ? bbox.min.y : bbox.max.y; 
				verts[i*4+j*2+k].z = k == 0 ? bbox.min.z : bbox.max.z; 
			}
		}
	}

	return result;
}

RenderContext::RenderContext()
{
	g_bufferManager = new BufferManager();
	m_defaultMat = new Material("_debug");
	m_mtrGlobalLight = new Material("_globallight");
	m_mtrGlobalLight->m_depthTest = true;
	m_mtrGlobalLight->m_depthWrite = false;
	//m_mtrGlobalLight->m
	m_hexahedron = 0;

#if AX_MTRENDER
	m_renderThread = new RenderThread();
	m_renderThread->startThread();
#endif
}

RenderContext::~RenderContext()
{
#if AX_MTRENDER
	m_renderThread->stopThread();
	SafeDelete(m_renderThread);
#endif
	SafeDelete(m_defaultMat);
	SafeDelete(g_bufferManager);
}


void RenderContext::issueFrame(RenderFrame *rq)
{
	double startTime = OsUtil::seconds();

	cacheFrame(rq);

	if (!r_specular.getBool()) {
		g_globalMacro.setMacro(GlobalMacro::G_DISABLE_SPECULAR);
	} else {
		g_globalMacro.resetMacro(GlobalMacro::G_DISABLE_SPECULAR);
	}

	m_curWindow = rq->getTarget();
	AX_ASSERT(m_curWindow->isWindow());
	// TODO: bind target

	int view_count = rq->getSceneCount();
	float frametime = rq->getScene(0)->camera.getFrameTime();

	RenderClearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true);
	//g_apiWrap->clear(clearer);

	float scenetime[16];
	bool m_isStatistic = false;

	for (int i = 0; i < view_count; i++) {
		if (i == view_count - 1) m_isStatistic = true;

		double s = OsUtil::seconds();
		RenderScene *queued = rq->getScene(i);

		drawScene(queued, clearer);
		clearer.clearColor(false);
		clearer.clearDepth(false);

		scenetime[i] = OsUtil::seconds() - s;
	}

	g_apiWrap->present(m_curWindow);

	g_apiWrap->issueDeletions();

	rq->clear();
}

void RenderContext::drawScene(RenderScene *scene, const RenderClearer &clearer)
{
	if (scene->sceneType == RenderScene::WorldMain) {
		drawScene_World(scene, clearer);
	} else if (scene->sceneType == RenderScene::Default) {
		drawScene_Noworld(scene, clearer);
	} else {
		Errorf("RenderContext::drawScene: error scene");
	}
}

#define BEGIN_PIX(x) g_apiWrap->beginPix(x)
#define END_PIX() g_apiWrap->endPix()

void RenderContext::drawScene_World(RenderScene *scene, const RenderClearer &clearer)
{
	BEGIN_PIX("DrawWorld");

	m_curTechnique = Technique::Main;
	m_curWorldScene = scene;

	const Rect &rect = scene->camera.getViewRect();
	int width = rect.width;
	int height = rect.height;

	AX_ST(RtDepth, m_curWindow->m_rtDepth->getTexture());
	AX_ST(Rt0, m_curWindow->m_rt0->getTexture());
	AX_ST(Rt1, m_curWindow->m_rt1->getTexture());
	AX_ST(Rt2, m_curWindow->m_rt2->getTexture());
	AX_ST(Rt3, m_curWindow->m_rt3->getTexture());

	// set global light parameter
	if (scene->globalLight) {
		AX_SU(g_globalLightPos, scene->globalLight->getOrigin());
		AX_SU(g_globalLightColor, scene->globalLight->getLightColor());
		AX_SU(g_skyColor, scene->globalLight->getSkyColor());
	}

	// set global fog
	if (scene->globalFog && r_fog.getBool()) {
		g_globalMacro.setMacro(GlobalMacro::G_FOG);
		AX_SU(g_fogParams, scene->globalFog->getFogParams());
	} else {
		g_globalMacro.resetMacro(GlobalMacro::G_FOG);
	}

	if (scene->waterFog && r_fog.getBool()) {
		AX_SU(g_waterFogParams, scene->waterFog->getFogParams());
	}

	AX_SU(g_windMatrices, scene->windMatrices);
	AX_SU(g_leafAngles, scene->leafAngles);

	float znear = scene->camera.getZnear();
	float zfar = scene->camera.getZfar();
	AX_SU(g_zrecoverParam, Vector4(znear, zfar, znear * zfar, znear - zfar));

	// draw subscene first
	for (int i = 0; i < scene->numSubScenes; i++) {
		RenderScene *sub = scene->subScenes[i];

		if (sub->sceneType == RenderScene::ShadowGen) {
			drawPass_ShadowGen(sub);
		} else if (sub->sceneType == RenderScene::Reflection) {
			BEGIN_PIX("ReflectionGen");
			g_globalMacro.setMacro(GlobalMacro::G_REFLECTION);
			//drawScene_WorldSub(sub);
			g_globalMacro.resetMacro(GlobalMacro::G_REFLECTION);
			END_PIX();
		} else if (sub->sceneType == RenderScene::RenderToTexture) {
			BEGIN_PIX("RenderToTexture");
			//drawScene_WorldSub(sub);
			END_PIX();
		}
	}

	// fill z first
	BEGIN_PIX("GfillPass");
	drawPass_GeoFill(scene);
	END_PIX();

	BEGIN_PIX("DrawLights");
	drawPass_Lights(scene);
	END_PIX();

	BEGIN_PIX("SceneComposite");
	//drawPass_Composite(scene);
	END_PIX();

	// post process and render back to backbuffer
	if (r_framebuffer.getBool()) {
		BEGIN_PIX("PostProcess");
		//drawPass_Postprocess(scene);
		END_PIX();
	}

	// check if need draw overlay primitives
	BEGIN_PIX("DrawOverlay");
	drawPass_Overlay(scene);
	END_PIX();

	END_PIX();
}

void RenderContext::drawScene_Noworld(RenderScene *scene, const RenderClearer &clearer)
{
	BEGIN_PIX("DrawNoworld");
	m_curTechnique = Technique::Main;

	m_targetSet.m_depthTarget = 0;
	m_targetSet.m_colorTargets[0] = scene->target;
	if (!m_targetSet.m_colorTargets[0])
		m_targetSet.m_colorTargets[0] = scene->camera.getTarget();
	m_targetSet.m_colorTargets[1] = m_targetSet.m_colorTargets[2] = m_targetSet.m_colorTargets[3] = 0;

	setupScene(scene, &clearer, 0);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	double start = OsUtil::seconds();
	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primtives[i]);
	}
	double end = OsUtil::seconds();

	//	unsetScene(scene, nullptr, scene->target);

	drawPass_Overlay(scene);

	if (m_isStatistic)
		stat_staticsTime.setInt((end - start) * 1000);
	END_PIX();
}

void RenderContext::drawPass_GeoFill(RenderScene *scene)
{
	m_curTechnique = Technique::GeoFill;

	RenderClearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true, Rgba::Zero);

	m_targetSet.m_depthTarget = m_curWindow->m_rtDepth;
	m_targetSet.m_colorTargets[0] = m_curWindow;
	m_targetSet.m_colorTargets[1] = m_curWindow->m_rt1;
	m_targetSet.m_colorTargets[2] = m_curWindow->m_rt2;
	m_targetSet.m_colorTargets[3] = m_curWindow->m_rt3;
	setupScene(scene, &clearer, 0);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	//	unsetScene(scene, &clearer, s_gbuffer);
}

void RenderContext::drawPass_Overlay(RenderScene *scene)
{
	if (!scene->numOverlayPrimitives) {
		return;
	}

	// draw overlay
	RenderCamera camera = scene->camera;
	camera.setOverlay(camera.getViewRect());

	m_targetSet.m_depthTarget = 0;
	m_targetSet.m_colorTargets[0] = m_curWindow->m_rt0;
	m_targetSet.m_colorTargets[1] = 0;
	m_targetSet.m_colorTargets[2] = 0;
	m_targetSet.m_colorTargets[3] = 0;

	setupScene(scene, 0, &camera);

	for (int i = 0; i < scene->numOverlayPrimitives; i++) {
		drawPrimitive(scene->overlayPrimitives[i]);
	}

	//	unsetScene(scene, nullptr, nullptr, &camera);
}

void RenderContext::drawPass_Composite(RenderScene *scene)
{
#if 1
	if (r_wireframe.getBool()) {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_forceWireframe = true;
	} else {
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_forceWireframe = false;
	}
#endif

	m_curTechnique = Technique::Main;

	m_targetSet.m_depthTarget = m_curWindow->m_rtDepth;
	m_targetSet.m_colorTargets[0] = m_curWindow->m_rt0;
	m_targetSet.m_colorTargets[1] = 0;
	m_targetSet.m_colorTargets[2] = 0;
	m_targetSet.m_colorTargets[3] = 0;

	setupScene(scene, 0, 0);

	issueVisQuery();

	if (!r_showLightBuf.getBool()) {
		RenderClearer clearer;
		clearer.clearDepth(false);
		clearer.clearColor(true, scene->clearColor);
		g_apiWrap->clear(clearer);

		for (int i = 0; i < scene->numInteractions; i++) {
			drawInteraction(scene->interactions[i]);
		}
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primtives[i]);
	}
	//unsetScene(scene, 0, scene->target);

#if 1
	//d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_forceWireframe = false;
#endif
}

void RenderContext::drawPass_ShadowGen(RenderScene *scene)
{
	if (!scene->numInteractions)
		return;

	RenderTarget *target = scene->camera.getTarget();

	Texture *tex = target->getTexture();

	RenderLight *qlight = scene->sourceLight;
	ShadowData *qshadow = qlight->m_shadowData;

	if (r_shadowGen.getBool()) {
		BEGIN_PIX("ShadowGen");

		m_rasterizerDesc.depthBias = true;

		m_curTechnique = Technique::ShadowGen;

		RenderClearer clearer;
		clearer.clearDepth(true);

		m_targetSet.m_depthTarget = target;
		m_targetSet.m_colorTargets[0] = 0;
		m_targetSet.m_colorTargets[1] = 0;
		m_targetSet.m_colorTargets[2] = 0;
		m_targetSet.m_colorTargets[3] = 0;

		setupScene(scene, 0, 0);
		g_apiWrap->clear(clearer);

		m_blendDesc.renderTargetWriteMask = 0;

		for (int i = 0; i < scene->numInteractions; i++) {
			drawInteraction(scene->interactions[i]);
		}

		m_blendDesc.renderTargetWriteMask = 0xf;

		if (scene->isLastCsmSplits())
			issueShadowQuery();

		//unsetScene(scene);

		// disable depth biase

		m_rasterizerDesc.depthBias = false;

		END_PIX();
	}

	scene->rendered = true;
}

void RenderContext::drawPass_Lights(RenderScene *scene)
{
	RenderClearer clearer;

	clearer.clearColor(true, Rgba::Zero);

	m_targetSet.m_depthTarget = m_curWindow->m_rtDepth;
	m_targetSet.m_colorTargets[0] = m_curWindow;
	m_targetSet.m_colorTargets[1] = 0;
	m_targetSet.m_colorTargets[2] = 0;
	m_targetSet.m_colorTargets[3] = 0;

	setupScene(m_curWorldScene, &clearer, 0);
	clearer.clearColor(false);

	for (int i = 0; i < scene->numLights; i++) {
		RenderLight *ql = scene->lights[i];

		if (ql->getLightType() == RenderLight::kGlobal) {
			drawGlobalLight(scene, ql);
		} else {
			//drawLocalLight(scene, ql);
		}
	}
}

void RenderContext::drawPass_Postprocess(RenderScene *scene)
{

}

void RenderContext::drawScene_WorldSub(RenderScene *scene)
{
	m_curTechnique = Technique::Main;

	RenderClearer clear;
	clear.clearDepth(true);
	clear.clearColor(true, scene->clearColor);

	// no shadow, no light, no fog etc. direct composite
	m_targetSet.m_depthTarget = 0;
	m_targetSet.m_colorTargets[0] = scene->target;
	m_targetSet.m_colorTargets[1] = 0;
	m_targetSet.m_colorTargets[2] = 0;
	m_targetSet.m_colorTargets[3] = 0;
	setupScene(scene, &clear, 0);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	for (int i = 0; i < scene->numDebugInteractions; i++) {
		drawInteraction(scene->debugInteractions[i]);
	}

	for (int i = 0; i < scene->numPrimitives; i++) {
		drawPrimitive(scene->primtives[i]);
	}
}

void RenderContext::drawPrimitive(Primitive *prim)
{
	m_curInteraction = 0;

	if (!prim)
		return;

	// check actor
	if (m_curEntity) {
		m_curEntity = 0;
		AX_SU(g_modelMatrix, Matrix::getIdentity());
	}

	prim->draw(Technique::Main);
}

void RenderContext::drawInteraction(Interaction *ia)
{
	static bool primMatrixSet = false;
	m_curInteraction = ia;

	Primitive *prim = ia->primitive;

	if (!prim) {
		return;
	}

	// check actor
	const RenderEntity *re = ia->entity;
	if (re != m_curEntity || prim->isMatrixSet() || primMatrixSet) {
		m_curEntity = re;

		if (m_curEntity) {
			if (prim->isMatrixSet()) {
				primMatrixSet = true;
			} else {
				primMatrixSet = false;
			}

			AX_SU(g_modelMatrix, m_curEntity->getMatrix());
			AX_SU(g_instanceParam, m_curEntity->getInstanceParam());

			if (prim->isMatrixSet()) {
				Matrix mat = prim->getMatrix().getAffineMat();
				mat = m_curEntity->getMatrix() * mat;
				AX_SU(g_modelMatrix, mat);
			}

			if (m_curEntity->getFlags() & RenderEntity::DepthHack) {
				//glDepthRange(0, 0.3f);
			} else {
				//glDepthRange(0, 1);
			}
		} else {
			AX_SU(g_modelMatrix, Matrix::getIdentity());
			AX_SU(g_instanceParam, Vector4(0,0,0,1));
		}
	}
	prim->draw(m_curTechnique);
}

static Matrix4 GetConvertMatrix()
{
	Matrix4 mat;
	mat.setIdentity();
	mat.scale(1, 1, 0.5);
	mat.translate(0, 0, 0.5);
	return mat;
}

static void ConvertToD3D(Matrix4 &m)
{
	static Matrix4 conv = GetConvertMatrix();

	if (g_renderDriverInfo.driverType == RenderDriverInfo::D3D) {
		m = conv * m;
	}
}

void RenderContext::setupScene(RenderScene *scene, const RenderClearer *clearer, RenderCamera *camera)
{
	if (!scene && !camera) {
		Errorf("RenderContext::setupScene: parameter error");
		return;
	}

	if (!camera)
		camera = &scene->camera;

//	m_curC = camera;
#if 0
	if (!target) {
		target = camera->getTarget();
	}

	bindTarget(target);
#else
	g_apiWrap->setTargetSet(m_targetSet);
#endif

	// clear here, before viewport and scissor set, so clear all render target's area,
	// if you want clear only viewport and scissor part, call clear after this function
	if (clearer) {
		//clearer->doClear();
		g_apiWrap->clear(*clearer);
	}

	g_apiWrap->setViewport(camera->getViewRect(), Vector2(0,1));
	g_apiWrap->setScissorRect(camera->getViewRect());

	AX_SU(g_time, (float)camera->getTime());

	Vector4 campos;
	campos.xyz() = camera->getOrigin();
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

	m_curInteraction = 0;
	m_curEntity = 0;

	AX_SU(g_modelMatrix, Matrix::getIdentity());
	AX_SU(g_instanceParam, Vector4(0,0,0,1));

	RenderTarget *target = m_targetSet.getFirstUsed();
	AX_ASSERT(target);
	Size r = target->getSize();
	AX_SU(g_sceneSize, Vector4(r.width,r.height,1.0f/r.width, 1.0f/r.height));

	if (camera->isReflectionEnabled()) {
		m_isReflecting = true;
	} else {
		m_isReflecting = false;
	}
}

void RenderContext::issueVisQuery()
{

}

void RenderContext::issueShadowQuery()
{

}


void RenderContext::drawMeshUP(Material *material, MeshUP *mesh)
{
	drawUP(mesh->m_vertices, VertexType::kChunk, mesh->m_numVertices, mesh->m_indices, ElementType_TriList, mesh->m_numIndices, material, Technique::Main);
}



#define F_SHADOWED 0
#define F_DIRECTION_LIGHT 1
#define F_SKY_LIGHT 2
#define F_ENV_LIGHT 3
void RenderContext::drawGlobalLight(RenderScene *scene, RenderLight *light)
{
	ShadowData *qshadow = light->m_shadowData;

	m_mtrGlobalLight->clearParameters();

	if (qshadow) {
		Texture *tex = qshadow->splitCameras[0].getTarget()->getTexture();
		Matrix4 matrix = qshadow->splitCameras[0].getViewProjMatrix();
		matrix.scale(0.5f, -0.5f, 0.5f);
		matrix.translate(0.5f, 0.5f, 0.5f);

		AX_ST(ShadowMap, tex);
		AX_SU(g_texMatrix, matrix);

		Size size = tex->size();
		Matrix4 fixmtx = *(Matrix4*)qshadow->splitScaleOffsets;

		float fixWidth = 0.5f / size.width;
		float fixHeight = 0.5f / size.height;

		fixmtx[0][2] += fixWidth;
		fixmtx[1][2] += fixWidth;
		fixmtx[2][2] += fixWidth;
		fixmtx[3][2] += fixWidth;
		fixmtx[0][3] = 1.0f - fixmtx[0][1] - fixmtx[0][3] + fixHeight;
		fixmtx[1][3] = 1.0f - fixmtx[1][1] - fixmtx[1][3] + fixHeight;
		fixmtx[2][3] = 1.0f - fixmtx[2][1] - fixmtx[2][3] + fixHeight;
		fixmtx[3][3] = 1.0f - fixmtx[3][1] - fixmtx[3][3] + fixHeight;
		AX_SU(g_csmOffsetScales, fixmtx.getTranspose());

		fixWidth *= 2;
		fixHeight *= 2;

		Matrix4 g_splitRanges = Matrix4(
			0,		0.5,	0.5,	1,
			0.5,	0.5,	1,		1,
			0,		0,		0.5,	0.5,
			0.5,	0,		1,		0.5
			);

		for (int i = 0; i < 4; i++) {
			g_splitRanges[i].xy() += Vector2(fixWidth, fixHeight);
			g_splitRanges[i].zw() -= Vector2(fixWidth*2, fixHeight*2);
			g_splitRanges[i] = (g_splitRanges[i] - fixmtx[i].zw()) / fixmtx[i].xy();
		}

		m_mtrGlobalLight->addParameter("s_splitRanges", 16, g_splitRanges.getTranspose().c_ptr());
		AX_SU(g_textureSize, Vector4(size.width, size.height, 1.0f/size.width, 1.0f/size.height));

		//m_mtrGlobalLight->setPixelToTexel(width, height);
	}

	MeshUP::setupHexahedron(m_hexahedron, light->m_lightVolume);

	Vector3 lightpos = light->m_affineMat.origin;
	lightpos.normalize();

	m_mtrGlobalLight->clearFeatures();
	m_mtrGlobalLight->setFeature(F_SHADOWED, light->m_shadowData != 0);
	m_mtrGlobalLight->setFeature(F_DIRECTION_LIGHT, !light->m_color.isZero());
	m_mtrGlobalLight->setFeature(F_SKY_LIGHT, !light->m_skyColor.isZero());
	m_mtrGlobalLight->setFeature(F_ENV_LIGHT, !light->m_envColor.isZero());
	m_mtrGlobalLight->addParameter("s_lightColor", 4, light->m_color.c_ptr());
	m_mtrGlobalLight->addParameter("s_skyColor", 3, light->m_skyColor.c_ptr());
	m_mtrGlobalLight->addParameter("s_envColor", 3, light->m_envColor.c_ptr());
	m_mtrGlobalLight->addParameter("s_lightPos", 4, lightpos.c_ptr());

	drawMeshUP(m_mtrGlobalLight, m_hexahedron);
}
#undef F_SHADOWED
#undef F_DIRECTION_LIGHT
#undef F_SKY_LIGHT
#undef F_ENV_LIGHT

void RenderContext::drawLocalLight(RenderScene *scene, RenderLight *light)
{

}


void RenderContext::drawUP( const void *vb, VertexType vt, int vertcount, const void *ib, ElementType et, int indicescount, Material *mat, Technique tech )
{
	if (!mat)
		mat = m_defaultMat;

	if (!mat->getShaderInfo()->m_haveTechnique[tech])
		return;

	const MaterialMacro &macro = mat->getShaderMacro();
	stat_numDrawElements.inc();

	g_apiWrap->setVerticesUP(vb, vt, vertcount);
	g_apiWrap->setIndicesUP(ib, et, indicescount);

	g_apiWrap->setShader(mat->getShaderName(), macro, tech);

	setMaterial(mat);

	setConstBuffers();

	if (mat->isWireframe() & !m_forceWireframe)
		m_rasterizerDesc.fillMode = RasterizerDesc::FillMode_Wireframe;

	g_apiWrap->draw();

	if (mat->isWireframe()  & !m_forceWireframe)
		m_rasterizerDesc.fillMode = RasterizerDesc::FillMode_Solid;
}

void RenderContext::draw(VertexObject *vert, InstanceObject *inst, IndexObject *index, Material *mat, Technique tech)
{
	if (!mat)
		mat = m_defaultMat;

	if (!mat->getShaderInfo()->m_haveTechnique[tech])
		return;

	MaterialMacro macro = mat->getShaderMacro();

	if (!inst) {
		g_apiWrap->setVertices(vert->m_h, vert->m_vt, vert->m_offset);
		g_globalMacro.resetMacro(GlobalMacro::G_GEOMETRY_INSTANCING);
	} else {
		g_apiWrap->setVerticesInstanced(vert->m_h, vert->m_vt, vert->m_offset, inst->m_h, inst->m_offset, inst->m_count);
		g_globalMacro.setMacro(GlobalMacro::G_GEOMETRY_INSTANCING);
	}

	if (!r_detail.getBool()) {
		// TODO
	}

	if (!r_bumpmap.getBool()) {
		macro.resetMacro(MaterialMacro::M_NORMAL);
	}

	stat_numDrawElements.inc();

	g_apiWrap->setIndices(index->m_h, index->m_elementType, index->m_offset, vert->m_count, index->m_activeCount);

	g_apiWrap->setShader(mat->getShaderName(), macro, tech);

	setMaterial(mat);

	setConstBuffers();

	if (mat->isWireframe() & !m_forceWireframe) {
		m_rasterizerDesc.fillMode = RasterizerDesc::FillMode_Wireframe;
	}

	g_apiWrap->draw();

	if (mat->isWireframe()  & !m_forceWireframe) {
		m_rasterizerDesc.fillMode = RasterizerDesc::FillMode_Solid;
	}
}

void RenderContext::setMaterial(Material *mat)
{
	if (!mat) {
		AX_SU(g_matDiffuse, Vector3(1,1,1));
		AX_SU(g_matSpecular, Vector3(0,0,0));
		AX_SU(g_matShiness, 10);
		return;
	}

	// set texgen parameters
	if (mat->isTexAnim()) {
		const Matrix4 *matrix = mat->getTexMatrix();
		if (matrix) {
			AX_SU(g_texMatrix, *matrix);
		}
	}

	// set material parameter
	AX_SU(g_matDiffuse, mat->getDiffuse());
	AX_SU(g_matSpecular, mat->getSpecular());
	AX_SU(g_matShiness, mat->getShiness());
	AX_SU(g_detailScale, mat->getDetailScale());

	m_depthStencilDesc.depthWritable = mat->m_depthWrite;
	m_depthStencilDesc.depthEnable = mat->m_depthTest;

	if (mat->m_twoSided)
		m_rasterizerDesc.cullMode = RasterizerDesc::CullMode_None;
	else
		m_rasterizerDesc.cullMode = RasterizerDesc::CullMode_Back;

	switch (mat->m_blendMode) {
	case Material::BlendMode_Disabled:
		m_blendDesc.blendEnable = false;
		break;
	case Material::BlendMode_Add:
		m_blendDesc.blendEnable = true;
		m_blendDesc.srcBlend = BlendDesc::BlendFactor_One;
		m_blendDesc.destBlend = BlendDesc::BlendFactor_One;
		break;
	case Material::BlendMode_Blend:
		m_blendDesc.blendEnable = true;
		m_blendDesc.srcBlend = BlendDesc::BlendFactor_SrcAlpha;
		m_blendDesc.destBlend = BlendDesc::BlendFactor_OneMinusSrcAlpha;
		break;
	case Material::BlendMode_Modulate:
		m_blendDesc.blendEnable = true;
		m_blendDesc.srcBlend = BlendDesc::BlendFactor_Zero;
		m_blendDesc.destBlend = BlendDesc::BlendFactor_SrcColor;
		break;
	}

	const FastParams *params = mat->getParameters();
	g_apiWrap->setParameters(0, params);

	// set material textures
	g_apiWrap->setMaterialTexture(mat->getTextureParams());
}


void RenderContext::setConstBuffers()
{
	for (int i = 0; i < ConstBuffer::MaxType; i++) {
		ConstBuffer *buffer = g_constBuffers.m_buffers[i];

		if (buffer->isDirty()) {
			g_apiWrap->setConstBuffer(buffer->getType(), buffer->getByteSize(), buffer->getDataPointer());
			buffer->clearDirty();
		}
	}

	if (m_depthStencilDesc != m_depthStencilDescLast || m_rasterizerDesc != m_rasterizerDescLast || m_blendDesc != m_blendDescLast) {
		g_apiWrap->setRenderState(m_depthStencilDesc, m_rasterizerDesc, m_blendDesc);
		m_depthStencilDescLast = m_depthStencilDesc;
		m_rasterizerDescLast = m_rasterizerDesc;
		m_blendDescLast = m_blendDesc;
	}
}


void RenderContext::cacheFrame(RenderFrame *queue)
{
	g_bufferManager->beginAlloc();

	for (int i = 0; i < queue->getSceneCount(); i++) {
		RenderScene *scene = queue->getScene(i);
		cacheScene(scene);
	}

	g_bufferManager->endAlloc();
}

void RenderContext::cacheScene(RenderScene *scene)
{
	float tangentlen = r_showTangents.getFloat();
	float normallen = r_showNormal.getFloat();

	for (int j = 0; j < scene->numInteractions; j++) {
		Primitive *prim = scene->interactions[j]->primitive;

		prim->sync();

		if (scene->sceneType != RenderScene::WorldMain)
			continue;

		if (scene->numDebugInteractions >= RenderScene::MAX_DEBUG_INTERACTIONS)
			continue;

		// check if need draw normal
		if (normallen > 0.00001f && prim->isMesh()) {
			MeshPrim *mesh = static_cast<MeshPrim*>(prim);

			LinePrim *line = mesh->getNormalLine(normallen);
			Interaction *ia = g_renderFrame->allocInteraction();
			ia->entity = scene->interactions[j]->entity;
			ia->primitive = line;
			ia->primitive->sync();

			scene->debugInteractions[scene->numDebugInteractions++] = ia;

			continue;
		}

		// check if need draw tangent space
		if (tangentlen > 0.00001f && prim->isMesh()) {
			MeshPrim *mesh = static_cast<MeshPrim*>(prim);

			LinePrim *line = mesh->getTangentLine(tangentlen);
			Interaction *ia = g_renderFrame->allocInteraction();
			ia->entity = scene->interactions[j]->entity;
			ia->primitive = line;
			ia->primitive->sync();

			scene->debugInteractions[scene->numDebugInteractions++] = ia;
		}
	}

	for (int i=0; i<scene->numDebugInteractions; i++) {
		scene->debugInteractions[i]->primitive->sync();
	}

	for (int j = 0; j < scene->numPrimitives; j++) {
		scene->primtives[j]->sync();
	}

	for (int j = 0; j < scene->numOverlayPrimitives; j++) {
		scene->overlayPrimitives[j]->sync();
	}
}

AX_END_NAMESPACE
