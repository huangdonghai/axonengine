#include "../private.h"

AX_BEGIN_NAMESPACE

void (*RenderApi::createTextureFromFileInMemory)(phandle_t h, IoRequest *asioRequest);
void (*RenderApi::createTexture2D)(phandle_t h, TexFormat format, int width, int height, int flags) = 0;
void (*RenderApi::uploadTexture)(phandle_t h, const void *pixels, TexFormat format);
void (*RenderApi::uploadSubTexture)(phandle_t h, const Rect &rect, const void *pixels, TexFormat format);
void (*RenderApi::generateMipmap)(phandle_t h);
void (*RenderApi::deleteTexture2D)(phandle_t h);
	  
void (*RenderApi::createVertexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
void (*RenderApi::uploadVertexBuffer)(phandle_t h, int datasize, const void *p);
void (*RenderApi::deleteVertexBuffer)(phandle_t h);
	  
void (*RenderApi::createIndexBuffer)(phandle_t h, int datasize, Primitive::Hint hint);
void (*RenderApi::uploadIndexBuffer)(phandle_t h, int datasize, const void *p);
void (*RenderApi::deleteIndexBuffer)(phandle_t h);

void (*RenderApi::createWindowTarget)(phandle_t h, Handle hwnd, int width, int height);
void (*RenderApi::updateWindowTarget)(phandle_t h, Handle newHwnd, int width, int height);
void (*RenderApi::deleteWindowTarget)(phandle_t h);

void (*RenderApi::setTargetSet)(phandle_t targetSet[RenderTargetSet::MaxTarget]);

void (*RenderApi::setViewport)(const Rect &rect, const Vector2 & depthRange);
void (*RenderApi::setScissorRect)(const Rect &scissorRect);

void (*RenderApi::setShader)(const FixedString & name, const ShaderMacro &sm, Technique tech);
void (*RenderApi::setConstBuffer)(ConstBuffers::Type type, int size, const void *data);
void (*RenderApi::setParameters)(const FastParams *params1, const FastParams *param2);
	  
void (*RenderApi::setVertices)(phandle_t h, VertexType vt, int vertcount);
void (*RenderApi::setInstanceVertices)(phandle_t h, VertexType vt, int vertcount, phandle_t inb, int inoffset, int incount);
void (*RenderApi::setIndices)(phandle_t h, ElementType et, int offset, int vertcount, int indicescount);

void (*RenderApi::setVerticesUP)(const void *vb, VertexType vt, int vertcount);
void (*RenderApi::setIndicesUP)(const void *ib, ElementType et, int indicescount);

void (*RenderApi::setGlobalTexture)(GlobalTextureId id, phandle_t h, const SamplerDesc &samplerState);
void (*RenderApi::setMaterialTexture)(phandle_t texs[], SamplerDesc samplerStates[]);

//	static void dip(ElementType et, int offset, int vertcount, int indices_count) = 0;
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
class ApiCommand_<void (*)(Arg0,Arg1)> : public ApiCommand {
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2)> : public ApiCommand {
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> : public ApiCommand {
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> : public ApiCommand {
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5)> : public ApiCommand {
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

static ApiCommand_<void (*)()> &sAllocCommand(void (*method)())
{
	typedef ApiCommand_<void (*)()> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0>
static ApiCommand_<void (*)(Arg0)> &sAllocCommand(void (*method)(Arg0))
{
	typedef ApiCommand_<void (*)(Arg0)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1>
static ApiCommand_<void (*)(Arg0,Arg1)> &sAllocCommand(void (*method)(Arg0,Arg1))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Rt, typename Arg0, typename Arg1, typename Arg2>
static ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> &sAllocCommand(Rt (*method)(Arg0,Arg1,Arg2))
{
	typedef ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> &sAllocCommand(void (*method)(Arg0,Arg1,Arg2,Arg3))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> &sAllocCommand(void (*method)(Arg0,Arg1,Arg2,Arg3,Arg4))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> ResultType;
	ResultType *result = g_apiWrap->allocCommand<ResultType>();
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5)> &sAllocCommand(void (*method)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4,Arg5)> ResultType;
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
	sAllocCommand(RenderApi::createTextureFromFileInMemory).args(h, asioRequest);
#else
	RenderApi::createTextureFromFileInMemory(h, asioRequest);
#endif
}

void ApiWrap::createTexture2D(phandle_t &h, TexFormat format, int width, int height, int flags /*= 0*/)
{
	h = newHandle();
#if AX_MTRENDER
	sAllocCommand(RenderApi::createTexture2D).args(h, format, width, height, flags);
#else
	RenderApi::createTexture2D(h, format, width, height, flags);
#endif
}

void ApiWrap::uploadTexture(phandle_t h, void *pixels, TexFormat format)
{
	// TODO
	AX_ASSERT(0);
	sAllocCommand(RenderApi::uploadTexture).args(h, pixels, format);
}

void ApiWrap::uploadSubTexture(phandle_t h, const Rect &rect, const void *pixels, TexFormat format)
{
#if AX_MTRENDER
	int datasize = format.calculateDataSize(rect.width, rect.height);
	void *newp = allocRingBuf(datasize);
	memcpy(newp, pixels, datasize);
	sAllocCommand(RenderApi::uploadSubTexture).args(h, rect, newp, format);
#else
	RenderApi::uploadSubTexture(h, rect, pixels, format);
#endif
}

void ApiWrap::generateMipmap(phandle_t h)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::generateMipmap).args(h);
#else
	RenderApi::generateMipmap(h);
#endif
}

void ApiWrap::deleteTexture2D(phandle_t h)
{
#if AX_MTRENDER
	addObjectDeletion(RenderApi::deleteTexture2D, h);
#else
	addObjectDeletion(RenderApi::deleteTexture2D, h);
#endif
}

void ApiWrap::createVertexBuffer(phandle_t &h, int datasize, Primitive::Hint hint)
{
	h = newHandle();
#if AX_MTRENDER
	sAllocCommand(RenderApi::createVertexBuffer).args(h, datasize, hint);
#else
	RenderApi::createVertexBuffer(h, datasize, hint);
#endif
}

void ApiWrap::uploadVertexBuffer(phandle_t h, int datasize, const void *p)
{
#if AX_MTRENDER
	void *newp = allocRingBuf(datasize);
	memcpy(newp, p, datasize);
	sAllocCommand(RenderApi::uploadVertexBuffer).args(h, datasize, newp);
#else
	RenderApi::uploadVertexBuffer(h, datasize, p);
#endif
}

void ApiWrap::deleteVertexBuffer(phandle_t h)
{
#if AX_MTRENDER
#else
#endif
	addObjectDeletion(RenderApi::deleteVertexBuffer, h);
}

void ApiWrap::createIndexBuffer(phandle_t &h, int datasize, Primitive::Hint hint)
{
	h = newHandle();
#if AX_MTRENDER
	sAllocCommand(RenderApi::createIndexBuffer).args(h, datasize, hint);
#else
	RenderApi::createIndexBuffer(h, datasize, hint);
#endif
}

void ApiWrap::uploadIndexBuffer(phandle_t h, int datasize, const void *p)
{
#if AX_MTRENDER
	void *newp = allocRingBuf(datasize);
	memcpy(newp, p, datasize);
	sAllocCommand(RenderApi::uploadIndexBuffer).args(h, datasize, newp);
#else
	RenderApi::uploadIndexBuffer(h, datasize, p);
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
	sAllocCommand(RenderApi::clear).args(clearer);
#else
	RenderApi::clear(clearer);
#endif
}

void ApiWrap::setTargetSet(const RenderTargetSet &targetSet)
{
#if AX_MTRENDER
	phandle_t *handles = allocType<phandle_t>(RenderTargetSet::MaxTarget);

	if (targetSet.m_depthTarget)
		handles[0] = targetSet.m_depthTarget->getPHandle();
	else
		handles[0] = 0;

	for (int i=0; i<RenderTargetSet::MaxColorTarget; i++) {
		if (targetSet.m_colorTargets[i])
			handles[i+1] = targetSet.m_colorTargets[i]->getPHandle();
		else
			handles[i+1] = 0;
	}
	sAllocCommand(RenderApi::setTargetSet).args(handles);
#else
	static phandle_t handles[RenderTargetSet::MaxTarget];
	if (targetSet.m_depthTarget)
		handles[0] = targetSet.m_depthTarget->getPHandle();
	else
		handles[0] = 0;

	for (int i=0; i<RenderTargetSet::MaxColorTarget; i++) {
		if (targetSet.m_colorTargets[i])
			handles[i+1] = targetSet.m_colorTargets[i]->getPHandle();
		else
			handles[i+1] = 0;
	}
	RenderApi::setTargetSet(handles);
#endif
}

void ApiWrap::addObjectDeletion(delete_func_t func, phandle_t h)
{
#if AX_MTRENDER
#else
#endif
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
	sAllocCommand(RenderApi::createWindowTarget).args(h, hwnd, width, height);
#else
	RenderApi::createWindowTarget(h, hwnd, width, height);
#endif
}

void ApiWrap::updateWindowTarget(phandle_t h, Handle newWndId, int width, int height)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::updateWindowTarget).args(h, newWndId, width, height);
#else
	RenderApi::updateWindowTarget(h, newWndId, width, height);
#endif
}

void ApiWrap::deleteWindowTarget(phandle_t h)
{
#if AX_MTRENDER
#else
#endif
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

	sAllocCommand(RenderApi::setParameters).args(newp1, newp2);
#else
	RenderApi::setParameters(params1, params2);
#endif
}
void ApiWrap::setShader(const FixedString &name, const ShaderMacro &sm, Technique tech)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::setShader).args(name, sm, tech);
#else
	RenderApi::setShader(name, sm, tech);
#endif
}

void ApiWrap::setConstBuffer(ConstBuffers::Type type, int size, const void *data)
{
#if AX_MTRENDER
	void *newData = allocRingBuf(size);
	memcpy(newData, data, size);
	sAllocCommand(RenderApi::setConstBuffer).args(type, size, newData);
#else
	RenderApi::setConstBuffer(type, size, data);
#endif
}

void ApiWrap::setGlobalTexture(GlobalTextureId gt, Texture *tex)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::setGlobalTexture).args(gt, tex->getPHandle(), tex->getSamplerState());
#else
	RenderApi::setGlobalTexture(gt, tex->getPHandle(), tex->getSamplerState());
#endif
}

void ApiWrap::setMaterialTexture(Texture * const tex[])
{
#if AX_MTRENDER
	phandle_t *handles = allocType<phandle_t>(MaterialTextureId::MaxType);
	SamplerDesc *descs = allocType<SamplerDesc>(MaterialTextureId::MaxType);

	for (int i = 0; i < MaterialTextureId::MaxType; i++) {
		if (tex[i]) {
			handles[i] = tex[i]->getPHandle();
			descs[i] = tex[i]->getSamplerState();
		} else {
			handles[i] = 0;
		}
	}

	sAllocCommand(RenderApi::setMaterialTexture).args(handles, descs);
#else
	static phandle_t handles[MaterialTextureId::MaxType];
	static SamplerDesc descs[MaterialTextureId::MaxType];

	for (int i = 0; i < MaterialTextureId::MaxType; i++) {
		if (tex[i]) {
			handles[i] = tex[i]->getPHandle();
			descs[i] = tex[i]->getSamplerState();
		} else {
			handles[i] = 0;
		}
	}

	RenderApi::setMaterialTexture(handles, descs);
#endif
}

void ApiWrap::setVerticesUP(const void *vb, VertexType vt, int vertcount)
{
#if AX_MTRENDER
	int size = vt.calcSize(vertcount);
	void *p = allocType<byte_t>(size);
	::memcpy(p, vb, size);
	sAllocCommand(RenderApi::setVerticesUP).args(p, vt, vertcount);
#else
	RenderApi::setVerticesUP(vb, vt, vertcount);
#endif
}

void ApiWrap::setIndicesUP(const void *ib, ElementType et, int indicescount)
{
#if AX_MTRENDER
	int size = indicescount * sizeof(ushort_t);
	void *p = allocType<byte_t>(size);
	::memcpy(p, ib, size);
	sAllocCommand(RenderApi::setIndicesUP).args(p, et, indicescount);
#else
	RenderApi::setIndicesUP(ib, et, indicescount);
#endif
}

void ApiWrap::setViewport(const Rect &rect, const Vector2 &depthRange)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::setViewport).args(rect, depthRange);
#else
	RenderApi::setViewport(rect, depthRange);
#endif
}

void ApiWrap::setScissorRect(const Rect &scissorRect)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::setScissorRect).args(scissorRect);
#else
	RenderApi::setScissorRect(scissorRect);
#endif
}


void ApiWrap::setVertices(phandle_t vb, VertexType vt, int offset)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::setVertices).args(vb, vt, offset);
#else
	RenderApi::setVertices(vb, vt, offset);
#endif
}

void ApiWrap::setVerticesInstanced(phandle_t vb, VertexType vt, int offset, phandle_t inb, int inoffset, int incount)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::setInstanceVertices).args(vb, vt, offset, inb, inoffset, incount);
#else
	RenderApi::setInstanceVertices(vb, vt, offset, inb, inoffset, incount);
#endif
}

void ApiWrap::setIndices(phandle_t ib, ElementType et, int offset, int vertcount, int indicescount)
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::setIndices).args(ib, et, offset, vertcount, indicescount);
#else
	RenderApi::setIndices(ib, et, offset, vertcount, indicescount);
#endif
}

void ApiWrap::draw()
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::draw).args();
#else
	RenderApi::draw();
#endif
}


void ApiWrap::present( RenderTarget *window )
{
#if AX_MTRENDER
	sAllocCommand(RenderApi::present).args(window->getPHandle());
#else
	RenderApi::present(window->getPHandle());
#endif
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


int ApiWrap::runCommands()
{
	int count = 0;

	while (1) {
		ApiCommand *cmd = fetchCommand();

		if (!cmd) {
			OsUtil::sleep(0);
			continue;
		}

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

void ApiWrap::deleteHandle( phandle_t h )
{
	delete h;
}


AX_END_NAMESPACE