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
	  
void (*RenderApi::setShader)(const FixedString & name, const ShaderMacro &sm, Technique tech);
void (*RenderApi::setVsConst)(const FixedString &name, int count, float *value);
void (*RenderApi::setPsConst)(const FixedString &name, int count, float *value);
	  
void (*RenderApi::setVertices)(phandle_t vb, VertexType vt, int vertcount);
void (*RenderApi::setInstanceVertices)(phandle_t vb, VertexType vt, int vertcount, Handle inb, int incount);
void (*RenderApi::setIndices)(phandle_t ib);

template <typename Signature>
class ApiCommand_ : public ApiWrap::Command
{
public:
	AX_STATIC_ASSERT(0);
};

template <>
class ApiCommand_<void (*)()> : public ApiWrap::Command
{
public:
	typedef void (*FunctionType)();

	ApiCommand_(FunctionType m)
		: m_m(m)
	{}

	void args()
	{
	}

	virtual void exec()
	{
		(m_m)();
	}

private:
	FunctionType m_m;
};

template <typename Arg0>
class ApiCommand_<void (*)(Arg0)> : public ApiWrap::Command
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
class ApiCommand_<void (*)(Arg0,Arg1)> : public ApiWrap::Command {
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
		(m_obj->*m_m)(m_arg0, m_arg1);
	}

private:
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
};

template <typename Arg0, typename Arg1, typename Arg2>
class ApiCommand_<void (*)(Arg0,Arg1,Arg2)> : public ApiWrap::Command {
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> : public ApiWrap::Command {
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
class ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> : public ApiWrap::Command {
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

static ApiCommand_<void (*)()> &AddCommand0(void (*method)())
{
	typedef ApiCommand_<void (*)()> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0>
static ApiCommand_<void (*)(Arg0)> &AddCommand1(void (*method)(Arg0))
{
	typedef ApiCommand_<void (*)(Arg0)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1>
static ApiCommand_<void (*)(Arg0,Arg1)> &AddCommand2(void (*method)(Arg0,Arg1))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Rt, typename Arg0, typename Arg1, typename Arg2>
static ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> &AddCommand3(Rt (*method)(Arg0,Arg1,Arg2))
{
	typedef ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> &AddCommand4(void (*method)(Arg0,Arg1,Arg2,Arg3))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
static ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> &AddCommand5(void (*method)(Arg0,Arg1,Arg2,Arg3,Arg4))
{
	typedef ApiCommand_<void (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

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
	AddCommand1(RenderApi::deleteTexture2D).args(h);
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
	AddCommand1(RenderApi::deleteVertexBuffer).args(h);
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
	AddCommand1(RenderApi::deleteIndexBuffer).args(h);
}

void ApiWrap::issueQueue( RenderQueue *rq )
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

	if (r_nulldraw.getBool()) {
		//goto endframe;
	}


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

void ApiWrap::beginFrame()
{

}

void ApiWrap::endFrame()
{

}

void ApiWrap::drawScene(QueuedScene *scene, const RenderClearer &clearer)
{

}

AX_END_NAMESPACE