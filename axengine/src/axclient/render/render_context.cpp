#include "../private.h"

AX_BEGIN_NAMESPACE

RenderContext::RenderContext()
{
	m_renderThread = new RenderThread();
	m_renderThread->startThread();

	g_bufferManager = new BufferManager();
	m_defaultMat = new Material("_debug");

	m_bufferWidth = m_bufferHeight = 0;
	m_depthBuffer = 0;
	m_geoBuffer = 0;
	m_lightBuffer = 0; // reuse as copied SceneColor
	m_sceneBuffer = 0;
}

RenderContext::~RenderContext()
{
	m_renderThread->stopThread();
	SafeDelete(m_renderThread);

	SafeDelete(m_defaultMat);
	SafeDelete(g_bufferManager);
}


void RenderContext::issueFrame(RenderFrame *rq)
{
	double startTime = OsUtil::seconds();

	cacheFrame(rq);

	beginFrame();

	m_frameWindow = rq->getTarget();
	AX_ASSERT(m_frameWindow->isWindow());
	// TODO: bind target

	int view_count = rq->getSceneCount();
	float frametime = rq->getScene(0)->camera.getFrameTime();

	RenderClearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true);
	g_apiWrap->clear(clearer);

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

	endFrame();

	g_apiWrap->issueDeletions();

	rq->clear();
}

void RenderContext::beginFrame()
{
	if (!r_specular.getBool()) {
		g_shaderMacro.setMacro(ShaderMacro::G_DISABLE_SPECULAR);
	} else {
		g_shaderMacro.resetMacro(ShaderMacro::G_DISABLE_SPECULAR);
	}
}

void RenderContext::endFrame()
{

}

void RenderContext::drawScene(RenderScene *scene, const RenderClearer &clearer)
{
	if (scene->sceneType == RenderScene::WorldMain) {
		drawScene_world(scene, clearer);
	} else if (scene->sceneType == RenderScene::Default) {
		drawScene_noworld(scene, clearer);
	} else {
		Errorf("RenderContext::drawScene: error scene");
	}
}

#define BEGIN_PIX(x)
#define END_PIX()
#define AX_SU(a,b) setUniform(ConstBuffers::a, b);
#define AX_ST(a,b) g_apiWrap->setGlobalTexture(GlobalTextureId::a, b)

void RenderContext::drawScene_world(RenderScene *scene, const RenderClearer &clearer)
{
	BEGIN_PIX("DrawWorld");

	m_technique = Technique::Main;

	const Rect &rect = scene->camera.getViewRect();
	int width = rect.width;
	int height = rect.height;

	if (1) {
		m_geoBuffer = m_frameWindow->getGeoBuffer();
		AX_ST(GeoBuffer, m_geoBuffer->getTexture());

		m_lightBuffer = m_frameWindow->getLightBuffer();
		AX_ST(LightBuffer, m_lightBuffer->getTexture());

	} else {
		m_worldRt = 0;
	}

	// set exposure
	float exposure = scene->exposure;
	if (exposure == 0) {
		g_shaderMacro.resetMacro(ShaderMacro::G_HDR);
		exposure = 1;
	} else {
		g_shaderMacro.setMacro(ShaderMacro::G_HDR);
	}

	stat_exposure.setInt(exposure * 100);

	AX_SU(g_exposure, Vector4(1.0f/exposure, exposure,0,0));

	// set global light parameter
	if (scene->globalLight) {
		AX_SU(g_globalLightPos, scene->globalLight->getOrigin());
		AX_SU(g_globalLightColor, scene->globalLight->getLightColor());
		AX_SU(g_skyColor, scene->globalLight->getSkyColor());
	}

	// set global fog
	if (scene->globalFog && r_fog.getBool()) {
		g_shaderMacro.setMacro(ShaderMacro::G_FOG);
		AX_SU(g_fogParams, scene->globalFog->getFogParams());
	} else {
		g_shaderMacro.resetMacro(ShaderMacro::G_FOG);
	}

	if (scene->waterFog && r_fog.getBool()) {
		AX_SU(g_waterFogParams, scene->waterFog->getFogParams());
	}

	AX_SU(g_windMatrices, scene->windMatrices);
	AX_SU(g_leafAngles, scene->leafAngles);

	// draw subscene first
	for (int i = 0; i < scene->numSubScenes; i++) {
		RenderScene *sub = scene->subScenes[i];

		if (sub->sceneType == RenderScene::ShadowGen) {
			drawPass_shadowGen(sub);
		} else if (sub->sceneType == RenderScene::Reflection) {
			BEGIN_PIX("ReflectionGen");
			//			g_shaderMacro.setMacro(ShaderMacro::G_REFLECTION);
			drawScene_worldSub(sub);
			//			g_shaderMacro.resetMacro(ShaderMacro::G_REFLECTION);
			END_PIX();
		} else if (sub->sceneType == RenderScene::RenderToTexture) {
			BEGIN_PIX("RenderToTexture");
			drawScene_worldSub(sub);
			END_PIX();
		}
	}

	// fill z first
	BEGIN_PIX("GfillPass");
	drawPass_gfill(scene);
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

void RenderContext::drawScene_noworld(RenderScene *scene, const RenderClearer &clearer)
{
	BEGIN_PIX("DrawNoworld");
	m_technique = Technique::Main;

	setupScene(scene, &clearer, scene->target);

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

	drawPass_overlay(scene);

	if (m_isStatistic)
		stat_staticsTime.setInt((end - start) * 1000);
	END_PIX();
}

void RenderContext::drawPass_gfill(RenderScene *scene)
{
	m_technique = Technique::Zpass;

	RenderClearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true, Rgba::Zero);

	setupScene(scene, &clearer, m_geoBuffer);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

	//	unsetScene(scene, &clearer, s_gbuffer);
}

void RenderContext::drawPass_overlay(RenderScene *scene)
{
	if (!scene->numOverlayPrimitives) {
		return;
	}

	// draw overlay
	RenderCamera camera = scene->camera;
	camera.setOverlay(camera.getViewRect());

	setupScene(scene, nullptr, nullptr, &camera);

	for (int i = 0; i < scene->numOverlayPrimitives; i++) {
		drawPrimitive(scene->overlayPrimitives[i]);
	}

	//	unsetScene(scene, nullptr, nullptr, &camera);
}

void RenderContext::drawPass_composite(RenderScene *scene)
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

	m_technique = Technique::Main;

	setupScene(scene, 0, scene->target);

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

void RenderContext::drawPass_shadowGen(RenderScene *scene)
{
	if (!scene->numInteractions)
		return;

	RenderTarget *target = scene->camera.getTarget();

	Texture *tex = target->getTexture();

	RenderLight *qlight = scene->sourceLight;
	QueuedShadow *qshadow = qlight->getQueuedShadow();

	if (r_shadowGen.getBool()) {
		BEGIN_PIX("ShadowGen");
		// offset the geometry slightly to prevent z-fighting
		// note that this introduces some light-leakage artifacts
		float factor = gl_shadowOffsetFactor.getFloat();
		float units = gl_shadowOffsetUnits.getFloat() / 0x10000;

#if 0
		d3d9StateManager->SetRenderState(D3DRS_DEPTHBIAS, F2DW(units));
		d3d9StateManager->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(factor));
#endif
		m_technique = Technique::ShadowGen;

		RenderClearer clearer;
		clearer.clearDepth(true);

		setupScene(scene, 0);
		g_apiWrap->clear(clearer);

#if 0
		d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
#endif
		for (int i = 0; i < scene->numInteractions; i++) {
			drawInteraction(scene->interactions[i]);
		}
#if 0
		d3d9StateManager->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);
#endif
		if (scene->isLastCsmSplits())
			issueShadowQuery();

		//unsetScene(scene);

		// disable depth biase
#if 0
		d3d9StateManager->SetRenderState(D3DRS_DEPTHBIAS, 0);
		d3d9StateManager->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
#endif
		END_PIX();
	}

#if 0
	tex->setHardwareShadowMap(true);
	tex->setFilterMode(Texture::FM_Linear);
	tex->setClampMode(Texture::CM_Clamp);
	tex->setBorderColor(Rgba::White);
#endif

	scene->rendered = true;
}

void RenderContext::drawPass_lights(RenderScene *scene)
{
	RenderClearer clearer;

	clearer.clearColor(true, Rgba::Zero);

	if (r_showLightBuf.getBool())
		m_lightBuffer = 0;

	setupScene(m_worldScene, &clearer, m_lightBuffer);
	clearer.clearColor(false);

	for (int i = 0; i < scene->numLights; i++) {
		RenderLight *ql = scene->lights[i];

		if (ql->getLightType() == RenderLight::kGlobal) {
			drawGlobalLight(scene, ql);
		} else {
			drawLocalLight(scene, ql);
		}
	}

	//unsetScene(d3d9WorldScene, nullptr, s_lbuffer);
}

void RenderContext::drawPass_postprocess(RenderScene *scene)
{

}

void RenderContext::drawScene_worldSub(RenderScene *scene)
{
	m_technique = Technique::Main;

	RenderClearer clear;
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
		drawPrimitive(scene->primtives[i]);
	}

	//	unsetScene(scene, &clear);
}

void RenderContext::drawPrimitive(Primitive *prim)
{
	m_ia = 0;

	if (!prim)
		return;

	// check actor
	if (m_entity) {
		m_entity = 0;
		AX_SU(g_modelMatrix, Matrix::getIdentity());
	}

	prim->draw(Technique::Main);
}

void RenderContext::drawInteraction(Interaction *ia)
{
	static bool primMatrixSet = false;
	m_ia = ia;

	Primitive *prim = ia->primitive;

	if (!prim) {
		return;
	}

	// check actor
	const RenderEntity *re = ia->entity;
	if (re != m_entity || prim->isMatrixSet() || primMatrixSet) {
		m_entity = re;

		if (m_entity) {
			if (prim->isMatrixSet()) {
				primMatrixSet = true;
			} else {
				primMatrixSet = false;
			}

			AX_SU(g_modelMatrix, m_entity->getMatrix());
			AX_SU(g_instanceParam, m_entity->getInstanceParam());

			if (prim->isMatrixSet()) {
				Matrix mat = prim->getMatrix().getAffineMat();
				mat = m_entity->getMatrix() * mat;
				AX_SU(g_modelMatrix, mat);
			}

			if (m_entity->getFlags() & RenderEntity::DepthHack) {
				//glDepthRange(0, 0.3f);
			} else {
				//glDepthRange(0, 1);
			}
		} else {
			AX_SU(g_modelMatrix, Matrix::getIdentity());
			AX_SU(g_instanceParam, Vector4(0,0,0,1));
		}
	}
	prim->draw(m_technique);
}

void RenderContext::setupScene(RenderScene *scene, const RenderClearer *clearer /*= 0*/, RenderTarget *target /*= 0*/, RenderCamera *camera /*= 0*/)
{

}

void RenderContext::issueVisQuery()
{

}

void RenderContext::issueShadowQuery()
{

}

void RenderContext::drawGlobalLight(RenderScene *scene, RenderLight *light)
{

}

void RenderContext::drawLocalLight(RenderScene *scene, RenderLight *light)
{

}

void RenderContext::draw(VertexObject *vert, InstanceObject *inst, IndexObject *index, Material *mat, Technique tech)
{
	if (!mat)
		mat = m_defaultMat;

	if (!mat->getShaderInfo()->m_haveTechnique[tech])
		return;

	ShaderMacro macro = g_shaderMacro;
	const ShaderMacro &matmacro = mat->getShaderMacro();
	macro.mergeFrom(&matmacro);

	if (!inst) {
		g_apiWrap->setVertices(vert->m_h, vert->m_vt, vert->m_offset);
		macro.resetMacro(ShaderMacro::G_GEOMETRY_INSTANCING);
	} else {
		g_apiWrap->setVerticesInstanced(vert->m_h, vert->m_vt, vert->m_offset, inst->m_h, inst->m_offset, inst->m_count);
		macro.setMacro(ShaderMacro::G_GEOMETRY_INSTANCING);
	}

	if (!r_detail.getBool() && mat->haveDetail()) {
		macro.resetMacro(ShaderMacro::G_HAVE_DETAIL);
		macro.resetMacro(ShaderMacro::G_HAVE_DETAIL_NORMAL);
	}

	if (!r_bumpmap.getBool()) {
		macro.resetMacro(ShaderMacro::G_HAVE_NORMAL);
	}

#if 0
	AX_SU(g_lightMap,  prim->m_lightmap);
	if (prim->m_lightmap && r_lightmap.getBool()) {
		macro.setMacro(ShaderMacro::G_HAVE_LIGHTMAP);
	} else {
		macro.resetMacro(ShaderMacro::G_HAVE_LIGHTMAP);
	}
#endif
	stat_numDrawElements.inc();

#if 0
	if (mat->isWireframe() & !d3d9ForceWireframe) {
		d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	draw(shader, tech, prim);

	if (mat->isWireframe()  & !d3d9ForceWireframe) {
		d3d9StateManager->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
#endif

	g_apiWrap->setIndices(index->m_h, index->m_elementType, index->m_offset, vert->m_count, index->m_count);

	g_apiWrap->setShader(mat->getShaderName(), macro, tech);

	setMaterialUniforms(mat);

	g_apiWrap->draw();
}

void RenderContext::setMaterialUniforms(Material *mat)
{
	if (!mat) {
		AX_SU(g_matDiffuse, Vector3(1,1,1));
		AX_SU(g_matSpecular, Vector3(0,0,0));
		AX_SU(g_matShiness, 10);
		return;
	}

	// set texgen parameters
	if (mat->isBaseTcAnim()) {
		const Matrix4 *matrix = mat->getBaseTcMatrix();
		if (matrix) {
			AX_SU(g_texMatrix, *matrix);
		}
	}

	// set material parameter
	AX_SU(g_matDiffuse, mat->getMatDiffuse());
	AX_SU(g_matSpecular, mat->getMatSpecular());
	AX_SU(g_matShiness, mat->getMatShiness());

	if (mat->haveDetail()) {
		float scale = mat->getDetailScale();
		Vector2 scale2(scale, scale);
		AX_SU(g_layerScale, scale2);
	}

#if 0
	const ShaderParams &params = mat->getParameters();

	ShaderParams::const_iterator it = params.begin();
	for (; it != params.end(); ++it) {
		const FloatSeq &value = it->second;
		g_apiWrap->setShaderConst(it->first, value.size() * sizeof(float),  &value[0]);
	}
#else
	const FastParams *params = mat->getParameters();
	g_apiWrap->setParameters(0, params);
#endif

	// set material textures
	g_apiWrap->setMaterialTexture(mat->getTextures());
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

void RenderContext::checkBufferSize(int width, int height)
{
	if (width <= m_bufferWidth && height <= m_bufferHeight)
		return;


}


AX_END_NAMESPACE