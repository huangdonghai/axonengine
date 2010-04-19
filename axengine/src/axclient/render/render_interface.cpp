#include "../private.h"

AX_BEGIN_NAMESPACE

template <typename Signature>
class ApiCommand_ : public ApiWrapper::Command
{
public:
	AX_STATIC_ASSERT(0);
};

template <typename Rt>
class ApiCommand_<Rt (*)()> : public ApiWrapper::Command
{
public:
	typedef Rt (*FunctionType)();

	ApiCommand_(FunctionType m)
		: m_m(m)
		, m_rt(0)
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
	Rt *m_rt;
};

template <typename Rt, typename Arg0>
class ApiCommand_<Rt (*)(Arg0)> : public ApiWrapper::Command
{
public:
	typedef Rt (*FunctionType)(Arg0);

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
	Rt *m_rt;
	typename remove_const_reference<Arg0>::type m_arg0;
};

template <typename Rt, typename Arg0, typename Arg1>
class ApiCommand_<Rt (*)(Arg0,Arg1)> : public ApiWrapper::Command {
public:
	typedef Rt (*FunctionType)(Arg0,Arg1);

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
	Rt *m_rt;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
};

template <typename Rt, typename Arg0, typename Arg1, typename Arg2>
class ApiCommand_<Rt (*)(Arg0,Arg1,Arg2)> : public ApiWrapper::Command {
public:
	typedef Rt (*FunctionType)(Arg0,Arg1,Arg2);

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
		if (m_rt)
			(*m_rt) = (m_m)(m_arg0, m_arg1, m_arg2);
		else
			(m_m)(m_arg0, m_arg1, m_arg2);
	}

private:
	FunctionType m_m;
	Rt *m_rt;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
};

template <typename Rt, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
class ApiCommand_<Rt (*)(Arg0,Arg1,Arg2,Arg3)> : public ApiWrapper::Command {
public:
	typedef Rt (*FunctionType)(Arg0,Arg1,Arg2,Arg3);

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

	void argsRt(Rt *rt, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		m_rt = rt;
		m_arg0 = arg0; m_arg1 = arg1; m_arg2 = arg2; m_arg3 = arg3;
	}

	virtual void exec()
	{
		(m_m)(m_arg0, m_arg1, m_arg2, m_arg3);
	}

private:
	FunctionType m_m;
	Rt *m_rt;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
};


template <typename Rt, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class ApiCommand_<Rt (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> : public ApiWrapper::Command {
public:
	typedef Rt (*FunctionType)(Arg0,Arg1,Arg2,Arg3,Arg4);

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
	Rt *m_rt;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
	typename remove_const_reference<Arg4>::type m_arg4;
};

template <typename Rt>
static ApiCommand_<Rt (*)()> &AddCommand0(Rt (*method)())
{
	typedef ApiCommand_<Rt (*)()> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Rt, typename Arg0>
static ApiCommand_<Rt (*)(Arg0)> &AddCommand1(Rt (*method)(Arg0))
{
	typedef ApiCommand_<Rt (*)(Arg0)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Rt, typename Arg0, typename Arg1>
static ApiCommand_<Rt (*)(Arg0,Arg1)> &AddCommand2(Rt (*method)(Arg0,Arg1))
{
	typedef ApiCommand_<Rt (*)(Arg0,Arg1)> ResultType;
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

template <typename Rt, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
static ApiCommand_<Rt (*)(Arg0,Arg1,Arg2,Arg3)> &AddCommand4(Rt (*method)(Arg0,Arg1,Arg2,Arg3))
{
	typedef ApiCommand_<Rt (*)(Arg0,Arg1,Arg2,Arg3)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

template <typename Rt, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
static ApiCommand_<Rt (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> &AddCommand5(Rt (*method)(Arg0,Arg1,Arg2,Arg3,Arg4))
{
	typedef ApiCommand_<Rt (*)(Arg0,Arg1,Arg2,Arg3,Arg4)> ResultType;
	ResultType *result = g_renderQueue->allocType<ResultType>(1);
	new (result) ResultType(method);
	return *result;
}

void ApiWrapper::createTexture2D( hptr_t result, TexFormat format, int width, int height, int flags /*= 0*/ )
{
	AddCommand4(RenderApi::createTexture2D).argsRt(result, format, width, height, flags);
}

AX_END_NAMESPACE