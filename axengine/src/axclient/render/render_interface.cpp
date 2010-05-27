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

void (*RenderApi::createWindowTarget)(phandle_t h, Handle hwnd);
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
	  
void (*RenderApi::setVertices)(phandle_t vb, VertexType vt, int vertcount);
void (*RenderApi::setInstanceVertices)(phandle_t vb, VertexType vt, int vertcount, Handle inb, int incount);
void (*RenderApi::setIndices)(phandle_t ib);

class ApiCommand
{
public:
	ApiCommand() {}
	virtual ~ApiCommand() {}
	virtual void exec() = 0;
};


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
	m_readPos = m_writePos = 0;
	m_lastLinkPos = -1;

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

void ApiWrap::uploadVertexBuffer(phandle_t h, int datasize, void *p)
{
	AddCommand3(RenderApi::uploadVertexBuffer).args(h, datasize, p);
}

void ApiWrap::deleteVertexBuffer(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteVertexBuffer, h);
}

void ApiWrap::createIndexBuffer(phandle_t result, int datasize, Primitive::Hint hint)
{
	AddCommand3(RenderApi::createIndexBuffer).args(result, datasize, hint);
}

void ApiWrap::uploadIndexBuffer(phandle_t h, int datasize, void *p)
{
	AddCommand3(RenderApi::uploadIndexBuffer).args(h, datasize, p);
}

void ApiWrap::deleteIndexBuffer(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteVertexBuffer, h);
}

void ApiWrap::clear( const RenderClearer &clearer )
{

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

void ApiWrap::createWindowTarget(phandle_t h, Handle hwnd)
{
	AddCommand2(RenderApi::createWindowTarget).args(h, hwnd);
}

void ApiWrap::updateWindowTarget(phandle_t h, Handle newWndId)
{

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

	if (m_writePos + size <= RING_BUFFER_SIZE) {
		waitToPos(m_writePos + size);
		result = m_ringBuffer + m_writePos;
		m_writePos += size;

		return result;
	}

	// wrap to begin
	if (m_writePos > size) {
		waitToPos(size);
		result = m_ringBuffer;
		m_writePos = size;

		return result;
	}

	// error, don't have enough space for allocate.
	Errorf("size is too large");
	return 0;
}

void ApiWrap::waitToPos(int pos)
{
	if (m_writePos >= m_readPos) {
		if (pos < m_readPos)
			return;

		if (pos > m_writePos)
			return;

		// do wait
		while (m_readPos < pos)
			OsUtil::sleep(0);
	} else {
		if (pos > m_writePos) {
			if (pos < m_readPos)
				return;
			while (m_readPos < pos)
				OsUtil::sleep(0);
		} else {
			while (m_readPos > m_writePos || m_readPos < pos)
				OsUtil::sleep(0);
		}
	}
}


void RenderContext::issueQueue(RenderQueue *rq)
{
	double startTime = OsUtil::seconds();

	beginFrame();

	RenderTarget *window = rq->getTarget();
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
		QueuedScene *queued = rq->getScene(i);

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

void RenderContext::drawScene(QueuedScene *scene, const RenderClearer &clearer)
{
	if (scene->sceneType == QueuedScene::WorldMain) {
		drawScene_world(scene, clearer);
	} else if (scene->sceneType == QueuedScene::Default) {
		drawScene_noworld(scene, clearer);
	} else {
		Errorf("D3D9Thread::drawScene: error scene");
	}
}

#define BEGIN_PIX(x)
#define END_PIX()
#define AX_SU(a,b)

void RenderContext::drawScene_world( QueuedScene *scene, const RenderClearer &clearer )
{
	BEGIN_PIX("DrawWorld");

	s_technique = Technique::Main;

	const Rect &rect = scene->camera.getViewRect();
	int width = rect.width;
	int height = rect.height;

	if (1) {
//		s_gbuffer = d3d9FrameWnd->m_gbuffer;
//		AX_SU(g_sceneDepth, s_gbuffer->getTextureDX());

//		s_lbuffer = d3d9FrameWnd->m_lightBuffer;
//		AX_SU(g_lightBuffer, s_lbuffer->getTextureDX());

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

void RenderContext::drawScene_noworld(QueuedScene *scene, const RenderClearer &clearer)
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

//	unsetScene(scene, nullptr, scene->target);

	drawPass_overlay(scene);

	if (m_isStatistic)
#if 0
		g_statistic->setValue(stat_staticsTime, (end - start) * 1000);
#else
		stat_staticsTime.setInt((end - start) * 1000);
#endif
	END_PIX();
}

void RenderContext::drawPass_gfill(QueuedScene *scene)
{
	s_technique = Technique::Zpass;

	RenderClearer clearer;
	clearer.clearDepth(true);
	clearer.clearColor(true, Rgba::Zero);

	setupScene(scene, &clearer, s_gbuffer);

	for (int i = 0; i < scene->numInteractions; i++) {
		drawInteraction(scene->interactions[i]);
	}

//	unsetScene(scene, &clearer, s_gbuffer);
}

void RenderContext::drawPass_overlay(QueuedScene *scene)
{

}

void RenderContext::drawPass_composite(QueuedScene *scene)
{

}

void RenderContext::drawPass_shadowGen(QueuedScene *scene)
{

}

void RenderContext::drawPass_lights(QueuedScene *scene)
{

}

void RenderContext::drawPass_postprocess(QueuedScene *scene)
{

}

void RenderContext::drawScene_worldSub(QueuedScene *scene)
{

}

void RenderContext::drawPrimitive( int prim_id )
{

}

void RenderContext::drawInteraction( Interaction *ia )
{

}

void RenderContext::setupScene( QueuedScene *scene, const RenderClearer *clearer /*= 0*/, RenderTarget *target /*= 0*/, RenderCamera *camera /*= 0*/ )
{

}

AX_END_NAMESPACE