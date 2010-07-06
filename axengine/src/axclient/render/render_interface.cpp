#include "../private.h"

AX_BEGIN_NAMESPACE

void (*RenderApi::createTexture2D)(phandle_t h, TexFormat format, int width, int height, int flags) = 0;
void (*RenderApi::uploadTexture)(phandle_t h, int level, void *pixels, TexFormat format);
void (*RenderApi::uploadSubTexture)(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
void (*RenderApi::generateMipmap)(phandle_t h);
void (*RenderApi::deleteTexture2D)(phandle_t h);
	  
void (*RenderApi::createVertexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
void (*RenderApi::uploadVertexBuffer)(phandle_t h, int datasize, void *p);
void (*RenderApi::deleteVertexBuffer)(phandle_t h);
	  
void (*RenderApi::createIndexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
void (*RenderApi::uploadIndexBuffer)(phandle_t h, int datasize, void *p);
void (*RenderApi::deleteIndexBuffer)(phandle_t h);

void (*RenderApi::createWindowTarget)(phandle_t h, Handle hwnd, int width, int height);
void (*RenderApi::updateWindowTarget)(phandle_t h, Handle newHwnd, int width, int height);
void (*RenderApi::deleteWindowTarget)(phandle_t h);

void (*RenderApi::createSamplerState)(phandle_t h, const SamplerStateDesc &samplerState);
void (*RenderApi::deleteSamplerState)(phandle_t h);

void (*RenderApi::createBlendState)(phandle_t h, const BlendStateDesc &src);
void (*RenderApi::deleteBlendState)(phandle_t h);

void (*RenderApi::createDepthStencilState)(phandle_t h, const DepthStencilStateDesc &src);
void (*RenderApi::deleteDepthStencilState)(phandle_t h);

void (*RenderApi::createRasterizerState)(phandle_t h, const RasterizerStateDesc &src);
void (*RenderApi::deleteRasterizerState)(phandle_t h);

void (*RenderApi::setShader)(const FixedString & name, const ShaderMacro &sm, Technique tech);
void (*RenderApi::setVsConst)(const FixedString &name, int count, float *value);
void (*RenderApi::setPsConst)(const FixedString &name, int count, float *value);
	  
void (*RenderApi::setVertices)(phandle_t h, VertexType vt, int vertcount);
void (*RenderApi::setInstanceVertices)(phandle_t h, VertexType vt, int vertcount, Handle inb, int incount);
void (*RenderApi::setIndices)(phandle_t h);

void (*RenderApi::clear)(const RenderClearer &clearer);


template <typename Signature>
class ApiCommand_ : public ApiCommand
{
public:
	AX_STATIC_ASSERT(0);
};

template <>
class ApiCommand_<void (*)()> : public ApiCommand
{
public:
	typedef void (*FunctionType)();

	ApiCommand_(FunctionType m)
		: m_m(m)
	{}

	void args()
	{}

	virtual void exec()
	{
		(m_m)();
	}

private:
	FunctionType m_m;
};

template <typename Arg0>
class ApiCommand_<void (*)(Arg0)> : public ApiCommand
{
public:
	typedef void (*FunctionType)(Arg0);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{}

	void args(Arg0 arg0)
	{
		m_arg0 = arg0;
	}

	virtual void exec()
	{
		(m_m)(m_arg0);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
};

template <typename Arg0, typename Arg1>
class ApiCommand_<void (*)(Arg0,Arg1)> : public ApiCommand {
public:
	typedef void (*FunctionType)(Arg0,Arg1);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{}

	void args(Arg0 arg0, Arg1 arg1)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
	}

	virtual void exec()
	{
		m_m(m_arg0, m_arg1);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
};

template <typename Arg0, typename Arg1, typename Arg2>
class ApiCommand_<void (*)(Arg0,Arg1,Arg2)> : public ApiCommand {
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{}

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
	}

	virtual void exec()
	{
		(m_m)(m_arg0, m_arg1, m_arg2);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
};

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> : public ApiCommand {
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2,Arg3);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{}

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
	}

	virtual void exec()
	{
		(m_m)(m_arg0, m_arg1, m_arg2, m_arg3);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
};


template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> : public ApiCommand {
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2,Arg3,Arg4);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{}

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
		m_arg4 = arg4;
	}

	virtual void exec()
	{
		(m_m)(m_arg0, m_arg1, m_arg2, m_arg3, m_arg4);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
	typename remove_const_reference<Arg4>::type m_arg4;
};

static void BeginCommand() {}
static void EndCommand() {}

static ApiCommand_<void (*)()> &AddCommand0(void (*method)())
{
	typedef ApiCommand_<void (*)()> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0>
static ApiCommand_<void (*)(Arg0)> &AddCommand1(void (*method)(Arg0))
{
	typedef ApiCommand_<void (*)(Arg0)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1>
static ApiCommand_<void (*)(Arg0,Arg1)> &AddCommand2(void (*method)(Arg0,Arg1))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Rt, typename Arg0, typename Arg1, typename Arg2>
static ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> &AddCommand3(Rt (*method)(Arg0,Arg1,Arg2))
{
	typedef ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> &AddCommand4(void (*method)(Arg0,Arg1,Arg2,Arg3))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> &AddCommand5(void (*method)(Arg0,Arg1,Arg2,Arg3,Arg4))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

ApiWrap::ApiWrap()
{
	m_bufReadPos = m_bufWritePos = 0;
	m_cmdReadPos = m_cmdWritePos = 0;

	m_numObjectDeletions = 0;
}

ApiWrap::~ApiWrap()
{}


void ApiWrap::createTexture2D(phandle_t result, TexFormat format, int width, int height, int flags /*= 0*/)
{
	AddCommand5(RenderApi::createTexture2D).args(result, format, width, height, flags);
}

void ApiWrap::uploadTexture( phandle_t h, int level, void *pixels, TexFormat format )
{
	AddCommand4(RenderApi::uploadTexture).args(h, level, pixels, format);
}

void ApiWrap::uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format)
{
	AddCommand4(RenderApi::uploadSubTexture).args(h, rect, pixels, format);
}

void ApiWrap::generateMipmap(phandle_t h)
{
	AddCommand1(RenderApi::generateMipmap).args(h);
}

void ApiWrap::deleteTexture2D(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteTexture2D, h);
}

void ApiWrap::createVertexBuffer(phandle_t result, int datasize, Primitive::Hint hint)
{
	AddCommand3(RenderApi::createVertexBuffer).args(result, datasize, hint);
}

void ApiWrap::uploadVertexBuffer(phandle_t h, int datasize, const void *p)
{
	void *newp = allocRingBuf(datasize);
	memcpy(newp, p, datasize);
	AddCommand3(RenderApi::uploadVertexBuffer).args(h, datasize, newp);
}

void ApiWrap::deleteVertexBuffer(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteVertexBuffer, h);
}

void ApiWrap::createIndexBuffer(phandle_t result, int datasize, Primitive::Hint hint)
{
	AddCommand3(RenderApi::createIndexBuffer).args(result, datasize, hint);
}

void ApiWrap::uploadIndexBuffer(phandle_t h, int datasize, const void *p)
{
	void *newp = allocRingBuf(datasize);
	memcpy(newp, p, datasize);
	AddCommand3(RenderApi::uploadIndexBuffer).args(h, datasize, newp);
}

void ApiWrap::deleteIndexBuffer(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteVertexBuffer, h);
}

void ApiWrap::clear(const RenderClearer &clearer)
{
	AddCommand1(RenderApi::clear).args(clearer);
}

void ApiWrap::createSamplerState(phandle_t h, const SamplerStateDesc &desc)
{
	AddCommand2(RenderApi::createSamplerState).args(h, desc);
}

void ApiWrap::deleteSamplerState(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteSamplerState, h);
}

void ApiWrap::createBlendState(phandle_t h, const BlendStateDesc &desc)
{
	AddCommand2(RenderApi::createBlendState).args(h, desc);
}

void ApiWrap::deleteBlendState(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteSamplerState, h);
}

void ApiWrap::createDepthStencilState(phandle_t h, const DepthStencilStateDesc &desc)
{
	AddCommand2(RenderApi::createDepthStencilState).args(h, desc);
}

void ApiWrap::deleteDepthStencilState(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteDepthStencilState, h);
}

void ApiWrap::createRasterizerState(phandle_t h, const RasterizerStateDesc &desc)
{
	AddCommand2(RenderApi::createRasterizerState).args(h, desc);
}

void ApiWrap::deleteRasterizerState(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteRasterizerState, h);
}

void ApiWrap::addObjectDeletion(delete_func_t func, phandle_t h)
{
	if (m_numObjectDeletions >= MAX_DELETE_COMMANDS) {
		Errorf("overflowed");
	}

	m_objectDeletions[m_numObjectDeletions].func = func;
	m_objectDeletions[m_numObjectDeletions].handle = h;

	m_numObjectDeletions++;
}

void ApiWrap::createWindowTarget(phandle_t h, Handle hwnd, int width, int height)
{
	AddCommand4(RenderApi::createWindowTarget).args(h, hwnd, width, height);
}

void ApiWrap::updateWindowTarget(phandle_t h, Handle newWndId, int width, int height)
{
	AddCommand4(RenderApi::updateWindowTarget).args(h, newWndId, width, height);
}

void ApiWrap::deleteWindowTarget(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteWindowTarget, h);
}

byte_t *ApiWrap::allocRingBuf(int size)
{
	// 4 bytes align
	size = (size + 3) & (~3);

	byte_t *result = 0;

	if (m_bufWritePos + size <= RING_BUFFER_SIZE) {
		waitToPos(m_bufWritePos + size);
		result = m_ringBuffer + m_bufWritePos;
		m_bufWritePos += size;

		return result;
	}

	// wrap to begin
	if (m_bufWritePos > size) {
		waitToPos(size);
		result = m_ringBuffer;
		m_bufWritePos = size;

		return result;
	}

	// error, don't have enough space for allocate.
	Errorf("size is too large");
	return 0;
}

void ApiWrap::waitToPos(int pos)
{
	if (m_bufWritePos >= m_bufReadPos) {
		if (pos < m_bufReadPos)
			return;

		if (pos > m_bufWritePos)
			return;

		// do wait
		while (m_bufReadPos < pos)
			OsUtil::sleep(0);
	} else {
		if (pos > m_bufWritePos) {
			if (pos < m_bufReadPos)
				return;
			while (m_bufReadPos < pos)
				OsUtil::sleep(0);
		} else {
			while (m_bufReadPos > m_bufWritePos || m_bufReadPos < pos)
				OsUtil::sleep(0);
		}
	}
}

void ApiWrap::setShaderConst(Uniforms::ItemName name, int size, const void *p)
{

}

void ApiWrap::setShaderConst(const FixedString &name, int size, const void *p)
{

}

void ApiWrap::setShader(const FixedString & name, const ShaderMacro &sm, Technique tech)
{

}

void ApiWrap::setVertices(phandle_t vb, VertexType vt, int offset)
{

}

void ApiWrap::setVerticesInstanced(phandle_t vb, VertexType vt, int offset, phandle_t inb, int incount)
{

}

void ApiWrap::setIndices(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount)
{

}

void ApiWrap::draw()
{

}

int ApiWrap::runCommands()
{
	int count = 0;

	while (1) {
		if (m_cmdReadPos == m_cmdWritePos)
			break;

		ApiCommand *cmd = m_ringCommand[m_cmdReadPos];
		cmd->exec();

		m_bufReadPos = cmd->m_bufPos;
		m_cmdReadPos = (m_cmdReadPos + 1) % ApiWrap::MAX_COMMANDS;

		cmd->~ApiCommand();
		count++;
	}

	return count;
}






RenderContext::RenderContext()
{
	m_defaultMat = new Material("_debug");
}

RenderContext::~RenderContext()
{
	SafeDelete(m_defaultMat);
}


void RenderContext::issueQueue(RenderFrame *rq)
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
		Errorf("D3D9Thread::drawScene: error scene");
	}
}

#define BEGIN_PIX(x)
#define END_PIX()
#define AX_SU(a,b) setUniform(UniformFields::a, b);

void RenderContext::drawScene_world(RenderScene *scene, const RenderClearer &clearer)
{
	BEGIN_PIX("DrawWorld");

	m_technique = Technique::Main;

	const Rect &rect = scene->camera.getViewRect();
	int width = rect.width;
	int height = rect.height;

	if (1) {
		m_gbuffer = m_frameWindow->getGBuffer();
		AX_SU(g_sceneDepth, m_gbuffer->getTexture());

		m_lbuffer = m_frameWindow->getLightBuffer();
		AX_SU(g_lightBuffer, m_lbuffer->getTexture());

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

	AX_SU(exposure, Vector4(1.0f/exposure, exposure,0,0));

	// set global light parameter
	if (scene->globalLight) {
		AX_SU(globalLightPos, scene->globalLight->getOrigin());
		AX_SU(globalLightColor, scene->globalLight->getLightColor());
		AX_SU(skyColor, scene->globalLight->getSkyColor());
	}

	// set global fog
	if (scene->globalFog && r_fog.getBool()) {
		g_shaderMacro.setMacro(ShaderMacro::G_FOG);
		AX_SU(fogParams, scene->globalFog->getFogParams());
	} else {
		g_shaderMacro.resetMacro(ShaderMacro::G_FOG);
	}

	if (scene->waterFog && r_fog.getBool()) {
		AX_SU(waterFogParams, scene->waterFog->getFogParams());
	}

	AX_SU(windMatrices, scene->windMatrices);
	AX_SU(leafAngles, scene->leafAngles);

	// draw subscene first
	for (int i = 0; i < scene->numSubScenes; i++) {
		RenderScene *sub = scene->subScenes[i];

		if (sub->sceneType == RenderScene::ShadowGen) {
			drawPass_shadowGen(sub);
		} else if (sub->sceneType == RenderScene::Reflection) {
			BEGIN_PIX("ReflectionGen");
			g_shaderMacro.setMacro(ShaderMacro::G_REFLECTION);
			drawScene_worldSub(sub);
			g_shaderMacro.resetMacro(ShaderMacro::G_REFLECTION);
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

	setupScene(scene, &clearer, m_gbuffer);

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
		m_lbuffer = 0;

	setupScene(m_worldScene, &clearer, m_lbuffer);
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
		AX_SU(modelMatrix, Matrix::getIdentity());
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

			AX_SU(modelMatrix, m_entity->getMatrix());
			AX_SU(instanceParam, m_entity->getInstanceParam());

			if (prim->isMatrixSet()) {
				Matrix mat = prim->getMatrix().getAffineMat();
				mat = m_entity->getMatrix() * mat;
				AX_SU(modelMatrix, mat);
			}

			if (m_entity->getFlags() & RenderEntity::DepthHack) {
				//glDepthRange(0, 0.3f);
			} else {
				//glDepthRange(0, 1);
			}
		} else {
			AX_SU(modelMatrix, Matrix::getIdentity());
			AX_SU(instanceParam, Vector4(0,0,0,1));
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
		g_apiWrap->setVerticesInstanced(vert->m_h, vert->m_vt, vert->m_offset, inst->m_h, inst->m_count);
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
		AX_SU(matDiffuse, Vector3(1,1,1));
		AX_SU(matSpecular, Vector3(0,0,0));
		AX_SU(matShiness, 10);
		return;
	}

	// set texgen parameters
	if (mat->isBaseTcAnim()) {
		const Matrix4 *matrix = mat->getBaseTcMatrix();
		if (matrix) {
			AX_SU(texMatrix, *matrix);
		}
	}

	// set material parameter
	AX_SU(matDiffuse, mat->getMatDiffuse());
	AX_SU(matSpecular, mat->getMatSpecular());
	AX_SU(matShiness, mat->getMatShiness());

	if (mat->haveDetail()) {
		float scale = mat->getDetailScale();
		Vector2 scale2(scale, scale);
		AX_SU(layerScale, scale2);
	}

	const ShaderParams &params = mat->getParameters();

	ShaderParams::const_iterator it = params.begin();
	for (; it != params.end(); ++it) {
		const FloatSeq &value = it->second;
		g_apiWrap->setShaderConst(it->first, value.size() * sizeof(float),  &value[0]);
	}
}

void RenderContext::setUniform(Uniforms::ItemName, Texture *texture)
{

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