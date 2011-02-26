#include "../private.h"

AX_BEGIN_NAMESPACE

void (*RenderApi::createTextureFromFileInMemory)(phandle_t h, IoRequest *asioRequest);
void (*RenderApi::createTexture)(phandle_t h, TexType type, TexFormat format, int width, int height, int depth, int flags);
void (*RenderApi::uploadSubTexture)(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
void (*RenderApi::generateMipmap)(phandle_t h);
void (*RenderApi::deleteTexture)(phandle_t h);
	  
void (*RenderApi::createVertexBuffer)(phandle_t h, int datasize, Primitive::Hint hint, const void *p);
void (*RenderApi::uploadVertexBuffer)(phandle_t h, int offset, int datasize, const void *p);
void (*RenderApi::deleteVertexBuffer)(phandle_t h);
	  
void (*RenderApi::createIndexBuffer)(phandle_t h, int datasize, Primitive::Hint hint, const void *p);
void (*RenderApi::uploadIndexBuffer)(phandle_t h, int offset, int datasize, const void *p);
void (*RenderApi::deleteIndexBuffer)(phandle_t h);

void (*RenderApi::createWindowTarget)(phandle_t h, Handle hwnd, int width, int height);
void (*RenderApi::updateWindowTarget)(phandle_t h, Handle newHwnd, int width, int height);
void (*RenderApi::deleteWindowTarget)(phandle_t h);

void (*RenderApi::issueQueries)(int n, Query *queries[]);

void (*RenderApi::beginPerfEvent)(const char *pixname);
void (*RenderApi::endPerfEvent)();

void (*RenderApi::setTargetSet)(phandle_t targetSet[RenderTargetSet::MaxTarget], int slices[RenderTargetSet::MaxTarget]);

void (*RenderApi::setViewport)(const Rect &rect, const Vector2 & depthRange);

void (*RenderApi::setShader)(const FixedString & name, const GlobalMacro &gm, const MaterialMacro &mm, Technique tech);
void (*RenderApi::setConstBuffer)(ConstBuffers::Type type, int size, const void *data);
void (*RenderApi::setParameters)(const FastParams *params1, const FastParams *param2);
	  
void (*RenderApi::setVertices)(phandle_t h, VertexType vt, int offset, int vertcount);
void (*RenderApi::setInstanceVertices)(phandle_t h, VertexType vt, int offset, int vertcount, phandle_t inb, int inoffset, int incount);
void (*RenderApi::setIndices)(phandle_t h, ElementType et, int offset, int indicescount);

void (*RenderApi::setVerticesUP)(const void *vb, VertexType vt, int vertcount);
void (*RenderApi::setIndicesUP)(const void *ib, ElementType et, int indicescount);

void (*RenderApi::setGlobalTexture)(GlobalTextureId id, phandle_t h, const SamplerDesc &samplerState);
void (*RenderApi::setMaterialTexture)(const FastTextureParams *textures);

void (*RenderApi::setRenderState)(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd);
void (*RenderApi::draw)();

void (*RenderApi::clear)(const RenderClearer &clearer);
void (*RenderApi::present)(phandle_t window);


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
	{ AX_ASSERT(m); }

	void args()
	{
		g_apiWrap->pushCommand(this);
	}

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
	{ AX_ASSERT(m); }

	void args(Arg0 arg0)
	{
		m_arg0 = arg0;
		g_apiWrap->pushCommand(this);
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
class ApiCommand_<void (*)(Arg0,Arg1)> : public ApiCommand
{
public:
	typedef void (*FunctionType)(Arg0,Arg1);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{ AX_ASSERT(m); }

	void args(Arg0 arg0, Arg1 arg1)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		g_apiWrap->pushCommand(this);
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2)> : public ApiCommand
{
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{ AX_ASSERT(m); }

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		g_apiWrap->pushCommand(this);
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> : public ApiCommand
{
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2,Arg3);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{ AX_ASSERT(m); }

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
		g_apiWrap->pushCommand(this);
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> : public ApiCommand
{
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2,Arg3,Arg4);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{ AX_ASSERT(m); }

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
		m_arg4 = arg4;
		g_apiWrap->pushCommand(this);
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

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5)> : public ApiCommand
{
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{ AX_ASSERT(m); }

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,Arg5 arg5)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
		m_arg4 = arg4;
		m_arg5 = arg5;
		g_apiWrap->pushCommand(this);
	}

	virtual void exec()
	{
		(m_m)(m_arg0, m_arg1, m_arg2, m_arg3, m_arg4, m_arg5);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
	typename remove_const_reference<Arg4>::type m_arg4;
	typename remove_const_reference<Arg5>::type m_arg5;
};

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)> : public ApiCommand
{
public:
	typedef void (*FunctionType)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6);

	ApiCommand_(FunctionType m)
		: m_m(m)
	{ AX_ASSERT(m); }

	void args(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4,Arg5 arg5,Arg6 arg6)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
		m_arg4 = arg4;
		m_arg5 = arg5;
		m_arg6 = arg6;
		g_apiWrap->pushCommand(this);
	}

	virtual void exec()
	{
		(m_m)(m_arg0, m_arg1, m_arg2, m_arg3, m_arg4, m_arg5, m_arg6);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
	typename remove_const_reference<Arg4>::type m_arg4;
	typename remove_const_reference<Arg5>::type m_arg5;
	typename remove_const_reference<Arg5>::type m_arg6;
};



static ApiCommand_<void (*)()> &AllocCommand_(void (*method)())
{
	typedef ApiCommand_<void (*)()> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0>
static ApiCommand_<void (*)(Arg0)> &AllocCommand_(void (*method)(Arg0))
{
	typedef ApiCommand_<void (*)(Arg0)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1>
static ApiCommand_<void (*)(Arg0,Arg1)> &AllocCommand_(void (*method)(Arg0,Arg1))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Rt, typename Arg0, typename Arg1, typename Arg2>
static ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> &AllocCommand_(Rt (*method)(Arg0,Arg1,Arg2))
{
	typedef ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> &AllocCommand_(void (*method)(Arg0,Arg1,Arg2,Arg3))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> &AllocCommand_(void (*method)(Arg0,Arg1,Arg2,Arg3,Arg4))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5)> &AllocCommand_(void (*method)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)> &AllocCommand_(void (*method)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)> ResultType;
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

void ApiWrap::createTextureFromFileInMemory(phandle_t &h, IoRequest *asioRequest)
{
	h = newHandle();
#if AX_MTRENDER
	AllocCommand_(RenderApi::createTextureFromFileInMemory).args(h, asioRequest);
#else
	RenderApi::createTextureFromFileInMemory(h, asioRequest);
#endif
}

void ApiWrap::createTexture(phandle_t &h, TexType type, TexFormat format, int width, int height, int depth, int flags)
{
	h = newHandle();
#if AX_MTRENDER
	AllocCommand_(RenderApi::createTexture).args(h, type, format, width, height, depth, flags);
#else
	RenderApi::createTexture(h, type, format, width, height, depth, flags);
#endif
}

void ApiWrap::uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format)
{
#if AX_MTRENDER
	AX_ASSERT(!rect.isEmpty());
	AX_ASSERT(format != TexFormat::AUTO);
	int datasize = format.calculateDataSize(rect.width, rect.height);
	void *newp = allocRingBuf(datasize);
	memcpy(newp, pixels, datasize);
	AllocCommand_(RenderApi::uploadSubTexture).args(h, rect, newp, format);
#else
	RenderApi::uploadSubTexture(h, rect, pixels, format);
#endif
}

void ApiWrap::generateMipmap(phandle_t h)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::generateMipmap).args(h);
#else
	RenderApi::generateMipmap(h);
#endif
}

void ApiWrap::deleteTexture(phandle_t h)
{
#if AX_MTRENDER
	addObjectDeletion(RenderApi::deleteTexture, h);
#else
	addObjectDeletion(RenderApi::deleteTexture, h);
#endif
}

void ApiWrap::createVertexBuffer(phandle_t &h, int datasize, Primitive::Hint hint, const void *p)
{
	h = newHandle();
#if AX_MTRENDER
	void *newP = 0;
	if (p) {
		newP = allocRingBuf(datasize);
		memcpy(newP, p, datasize);
	}
	AllocCommand_(RenderApi::createVertexBuffer).args(h, datasize, hint, newP);
#else
	RenderApi::createVertexBuffer(h, datasize, hint, p);
#endif
}

void ApiWrap::uploadVertexBuffer(phandle_t h, int offset, int datasize, const void *p)
{
#if AX_MTRENDER
	void *newp = allocRingBuf(datasize);
	memcpy(newp, p, datasize);
	AllocCommand_(RenderApi::uploadVertexBuffer).args(h, offset, datasize, newp);
#else
	RenderApi::uploadVertexBuffer(h, offset, datasize, p);
#endif
}

void ApiWrap::deleteVertexBuffer(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteVertexBuffer, h);
}

void ApiWrap::createIndexBuffer(phandle_t &h, int datasize, Primitive::Hint hint, const void *p)
{
	h = newHandle();
#if AX_MTRENDER
	void *newP = 0;
	if (p) {
		newP = allocRingBuf(datasize);
		memcpy(newP, p, datasize);
	}
	AllocCommand_(RenderApi::createIndexBuffer).args(h, datasize, hint, p);
#else
	RenderApi::createIndexBuffer(h, datasize, hint, p);
#endif
}

void ApiWrap::uploadIndexBuffer(phandle_t h, int offset, int datasize, const void *p)
{
#if AX_MTRENDER
	void *newp = allocRingBuf(datasize);
	memcpy(newp, p, datasize);
	AllocCommand_(RenderApi::uploadIndexBuffer).args(h, offset, datasize, newp);
#else
	RenderApi::uploadIndexBuffer(h, offset, datasize, p);
#endif
}

void ApiWrap::deleteIndexBuffer(phandle_t h)
{
#if AX_MTRENDER
#else
#endif
	addObjectDeletion(RenderApi::deleteVertexBuffer, h);
}

void ApiWrap::clear(const RenderClearer &clearer)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::clear).args(clearer);
#else
	RenderApi::clear(clearer);
#endif
}

void ApiWrap::issueQueries(int n, Query *queries[])
{
#if AX_MTRENDER
	Query **newData = allocType<Query *>(n);
	memcpy(newData, queries, n * sizeof(Query *));
	AllocCommand_(RenderApi::issueQueries).args(n, newData);
#else
	RenderApi::issueQueries(n, queries);
#endif
}

void ApiWrap::beginPerfEvent(const char *pixname)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::beginPerfEvent).args(pixname);
#else
	RenderApi::beginPerfEvent(pixname);
#endif
}

void ApiWrap::endPerfEvent()
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::endPerfEvent).args();
#else
	RenderApi::endPerfEvent();
#endif
}

void ApiWrap::setTargetSet(const RenderTargetSet &targetSet)
{
#if AX_MTRENDER
	phandle_t *handles = allocType<phandle_t>(RenderTargetSet::MaxTarget);
	int *newSlices = allocType<int>(RenderTargetSet::MaxTarget);
#else
	static phandle_t handles[RenderTargetSet::MaxTarget];
	static int newSlices[RenderTargetSet::MaxTarget];
#endif

	if (targetSet.m_depthTarget) {
		handles[0] = targetSet.m_depthTarget.target->getPHandle();
		newSlices[0] = targetSet.m_depthTarget.slice;
	} else {
		handles[0] = 0;
		newSlices[0] = 0;
	}

	for (int i=0; i<RenderTargetSet::MaxColorTarget; i++) {
		if (targetSet.m_colorTargets[i]) {
			handles[i+1] = targetSet.m_colorTargets[i].target->getPHandle();
			newSlices[i+1] = targetSet.m_colorTargets[i].slice;
		} else {
			handles[i+1] = 0;
			newSlices[i+1] = 0;
		}
	}

#if AX_MTRENDER
	AllocCommand_(RenderApi::setTargetSet).args(handles, newSlices);
#else
	RenderApi::setTargetSet(handles, newSlices);
#endif
}

void ApiWrap::addObjectDeletion(delete_func_t func, phandle_t h)
{
	if (m_numObjectDeletions >= MAX_DELETE_COMMANDS)
		Errorf("overflowed");

	m_objectDeletions[m_numObjectDeletions].func = func;
	m_objectDeletions[m_numObjectDeletions].handle = h;

	m_numObjectDeletions++;
}

void ApiWrap::createWindowTarget(phandle_t &h, Handle hwnd, int width, int height)
{
	h = newHandle();
#if AX_MTRENDER
	AllocCommand_(RenderApi::createWindowTarget).args(h, hwnd, width, height);
#else
	RenderApi::createWindowTarget(h, hwnd, width, height);
#endif
}

void ApiWrap::updateWindowTarget(phandle_t h, Handle newWndId, int width, int height)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::updateWindowTarget).args(h, newWndId, width, height);
#else
	RenderApi::updateWindowTarget(h, newWndId, width, height);
#endif
}

void ApiWrap::deleteWindowTarget(phandle_t h)
{
	addObjectDeletion(RenderApi::deleteWindowTarget, h);
}

void ApiWrap::setParameters(const FastParams *params1, const FastParams *params2)
{
#if AX_MTRENDER
	FastParams *newp1 = 0;
	FastParams *newp2 = 0;

	if (params1) {
		newp1 = allocType<FastParams>();
		*newp1 = *params1;
	}

	if (params2) {
		newp2 = allocType<FastParams>();
		*newp2 = *params2;
	}

	AllocCommand_(RenderApi::setParameters).args(newp1, newp2);
#else
	RenderApi::setParameters(params1, params2);
#endif
}
void ApiWrap::setShader(const FixedString &name, const MaterialMacro &mm, Technique tech)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::setShader).args(name, g_globalMacro, mm, tech);
#else
	RenderApi::setShader(name, g_globalMacro, mm, tech);
#endif
}

void ApiWrap::setConstBuffer(ConstBuffers::Type type, int size, const void *data)
{
#if AX_MTRENDER
	void *newData = allocRingBuf(size);
	memcpy(newData, data, size);
	AllocCommand_(RenderApi::setConstBuffer).args(type, size, newData);
#else
	RenderApi::setConstBuffer(type, size, data);
#endif
}

void ApiWrap::setGlobalTexture(GlobalTextureId gt, Texture *tex)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::setGlobalTexture).args(gt, tex->getPHandle(), tex->getSamplerState());
#else
	RenderApi::setGlobalTexture(gt, tex->getPHandle(), tex->getSamplerState());
#endif
}

void ApiWrap::setMaterialTexture(const FastTextureParams *textures)
{
#if AX_MTRENDER
	FastTextureParams *newp = allocType<FastTextureParams>(1);
	*newp = *textures;
	AllocCommand_(RenderApi::setMaterialTexture).args(newp);
#else
	RenderApi::setMaterialTexture(textures);
#endif
}

void ApiWrap::setVerticesUP(const void *vb, VertexType vt, int vertcount)
{
#if AX_MTRENDER
	int size = vt.calcSize(vertcount);
	void *p = allocRingBuf(size);
	::memcpy(p, vb, size);
	AllocCommand_(RenderApi::setVerticesUP).args(p, vt, vertcount);
#else
	RenderApi::setVerticesUP(vb, vt, vertcount);
#endif
}

void ApiWrap::setIndicesUP(const void *ib, ElementType et, int indicescount)
{
#if AX_MTRENDER
	int size = indicescount * sizeof(ushort_t);
	void *p = allocRingBuf(size);
	::memcpy(p, ib, size);
	AllocCommand_(RenderApi::setIndicesUP).args(p, et, indicescount);
#else
	RenderApi::setIndicesUP(ib, et, indicescount);
#endif
}

void ApiWrap::setViewport(const Rect &rect, const Vector2 &depthRange)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::setViewport).args(rect, depthRange);
#else
	RenderApi::setViewport(rect, depthRange);
#endif
}

void ApiWrap::setVertices(phandle_t vb, VertexType vt, int offset, int vert_count)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::setVertices).args(vb, vt, offset, vert_count);
#else
	RenderApi::setVertices(vb, vt, offset, vert_count);
#endif
}

void ApiWrap::setVerticesInstanced(phandle_t vb, VertexType vt, int offset, int vertcount, phandle_t inb, int inoffset, int incount)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::setInstanceVertices).args(vb, vt, offset, vertcount, inb, inoffset, incount);
#else
	RenderApi::setInstanceVertices(vb, vt, offset, vertcount, inb, inoffset, incount);
#endif
}

void ApiWrap::setIndices(phandle_t ib, ElementType et, int offset, int indicescount)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::setIndices).args(ib, et, offset, indicescount);
#else
	RenderApi::setIndices(ib, et, offset, indicescount);
#endif
}

void ApiWrap::setRenderState(const DepthStencilDesc &dsd, const RasterizerDesc &rd, const BlendDesc &bd)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::setRenderState).args(dsd, rd, bd);
#else
	RenderApi::setRenderState(dsd, rd, bd);
#endif
}

void ApiWrap::draw()
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::draw).args();
#else
	RenderApi::draw();
#endif
}


void ApiWrap::present(RenderTarget *window)
{
#if AX_MTRENDER
	AllocCommand_(RenderApi::present).args(window->getPHandle());
#else
	RenderApi::present(window->getPHandle());
#endif
}

size_t ApiWrap::calcStop( size_t size )
{
	return size & ~(RING_BUFFER_SIZE - 1);
}

size_t ApiWrap::calcPos(size_t size)
{
	return size & (RING_BUFFER_SIZE - 1);
}

byte_t *ApiWrap::allocRingBuf(int size)
{
	// 4 bytes align
	size = (size + 3) & (~3);

	size_t d0 = Math::distant(87, 104);
	size_t d1 = Math::distant(0xffffffff, 501);

	byte_t *result = 0;

	size_t oldReadPos = m_bufReadPos;
	size_t readStop = calcStop(oldReadPos);
	size_t writeStop = calcStop(m_bufWritePos);
	size_t nextstop = writeStop + RING_BUFFER_SIZE;

	if (calcStop(m_bufWritePos + size) == writeStop) {
		if (readStop == writeStop) {
			// do nothing
		} else {
			while (Math::distant(readStop, m_bufReadPos) < Math::distant(writeStop, m_bufWritePos + size)) {
				OsUtil::sleep(0);
			}
		}
		result = m_ringBuffer + calcPos(m_bufWritePos);
		m_bufWritePos += size;
	} else {
		if (size > calcPos(m_bufWritePos)) {
			Errorf("size is too large");
		}

		if (readStop == writeStop) {
			while (Math::distant(readStop, m_bufReadPos) < size) {
				OsUtil::sleep(0);
			}
		} else {
			while (Math::distant(readStop, m_bufReadPos) < Math::distant(writeStop, nextstop + size)) {
				OsUtil::sleep(0);
			}
		}

		result = m_ringBuffer;
		m_bufWritePos = nextstop + size;
	}
	int dist = Math::distant(oldReadPos, m_bufWritePos);
//	AX_ASSERT(dist <= RING_BUFFER_SIZE);

#if 0
	if (m_bufWritePos + size <= RING_BUFFER_SIZE) {
		waitToPos(m_bufWritePos + size);
		result = m_ringBuffer + m_bufWritePos;
		m_bufWritePos += size;
	} else if (m_bufWritePos > size) {	// wrap to begin
		waitToPos(size);
		result = m_ringBuffer;
		m_bufWritePos = size;
	}
#endif
	if (result) {
		memset(result, 0, size);
		return result;
	}

	// error, don't have enough space for allocate.
	Errorf("size is too large");
	return 0;
}

void ApiWrap::waitToPos(size_t pos)
{
	size_t readstop = (m_bufReadPos / RING_BUFFER_SIZE) * RING_BUFFER_SIZE;
	size_t writestop = (m_bufWritePos / RING_BUFFER_SIZE) * RING_BUFFER_SIZE;
	size_t newpos = calcPos(pos);
	size_t newWritePos = calcPos(m_bufWritePos);

	if (readstop == writestop) {
		if (newpos > newWritePos)
			return;

		while (calcPos(m_bufReadPos) < newpos)
			OsUtil::sleep(0);

		return;
	} else {
		if (newpos > newWritePos) {
			while (m_bufReadPos - readstop < newpos)
				OsUtil::sleep(0);
		} else {
			while (m_bufReadPos - readstop < newpos + RING_BUFFER_SIZE)
				OsUtil::sleep(0);
		}
	}

#if 0
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
#endif
}

void ApiWrap::pushCommand(ApiCommand *cmd)
{
	cmd->m_bufPos = m_bufWritePos;
	m_ringCommand[m_cmdWritePos % MAX_COMMANDS] = cmd;
	m_cmdWritePos++;
}

ApiCommand *ApiWrap::fetchCommand()
{
	while (isEmpty()) {
		OsUtil::sleep(0);
	}

	return m_ringCommand[m_cmdReadPos % MAX_COMMANDS];
}

void ApiWrap::popCommand()
{
	ApiCommand *cmd = m_ringCommand[m_cmdReadPos % MAX_COMMANDS];
	m_ringCommand[m_cmdReadPos % MAX_COMMANDS] = 0;
	int bufpos = cmd->m_bufPos;
	cmd->~ApiCommand();
	m_bufReadPos = bufpos;
	m_cmdReadPos++;
}

void ApiWrap::finish()
{
	while (m_cmdWritePos != m_cmdReadPos) {
		OsUtil::sleep(0);
	}
}

int ApiWrap::runCommands()
{
	int count = 0;

	while (1) {
		ApiCommand *cmd = fetchCommand();

		cmd->exec();

		popCommand();

		count++;
	}

	return count;
}

phandle_t ApiWrap::newHandle()
{
	return new Handle(0);
}

void ApiWrap::deleteHandle(phandle_t h)
{
	delete h;
}

void ApiWrap::issueDeletions()
{
#if AX_MTRENDER
	for (int i = 0; i < m_numObjectDeletions; i++) {
		AllocCommand_(m_objectDeletions[i].func).args(m_objectDeletions[i].handle);
	}
#else
	for (int i = 0; i < m_numObjectDeletions; i++) {
		m_objectDeletions[i].func(m_objectDeletions[i].handle);
	}
#endif

	m_numObjectDeletions = 0;
}

bool ApiWrap::isFull() const
{
	return Math::abs(m_cmdWritePos - m_cmdReadPos) == MAX_COMMANDS;
}


AX_END_NAMESPACE