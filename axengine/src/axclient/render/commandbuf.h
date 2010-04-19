#ifndef AX_COMMANDBUF_H
#define AX_COMMANDBUF_H

AX_BEGIN_NAMESPACE

class RenderData;

class SyncMethod
{
public:
	SyncMethod() { g_renderQueue->addDeferredCommand(this); }
	virtual ~SyncMethod() {}
	virtual void exec() = 0;
};

template <typename Signature>
class SyncMethod_ : public SyncMethod {
public:
	AX_STATIC_ASSERT(0);
};

template <typename Rt, typename T>
class SyncMethod_<Rt (T::*)()> : public SyncMethod {
public:
	typedef Rt (T::*FunctionType)();

	SyncMethod_(T *obj, FunctionType m)
		: m_obj(obj)
		, m_m(m)
	{}

	void call()
	{
	}

	virtual void exec()
	{
		(m_obj->*m_m)();
	}

private:
	T *m_obj;
	FunctionType m_m;
};

template <typename Rt, typename T, typename Arg0>
class SyncMethod_<Rt (T::*)(Arg0)> : public SyncMethod {
public:
	typedef Rt (T::*FunctionType)(Arg0);

	SyncMethod_(T *obj, FunctionType m)
		: m_obj(obj)
		, m_m(m)
	{}

	void call(Arg0 arg0)
	{
		m_arg0 = arg0;
	}

	virtual void exec()
	{
		(m_obj->*m_m)(m_arg0);
	}

private:
	T *m_obj;
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
};

template <typename Rt, typename T, typename Arg0, typename Arg1>
class SyncMethod_<Rt (T::*)(Arg0,Arg1)> : public SyncMethod {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1);

	SyncMethod_(T *obj, FunctionType m)
		: m_obj(obj)
		, m_m(m)
	{}

	void call(Arg0 arg0, Arg1 arg1)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
	}

	virtual void exec()
	{
		(m_obj->*m_m)(m_arg0, m_arg1);
	}

private:
	T *m_obj;
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
};

template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2>
class SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2)> : public SyncMethod {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1,Arg2);

	SyncMethod_(T *obj, FunctionType m)
		: m_obj(obj)
		, m_m(m)
	{}

	void call(Arg0 arg0, Arg1 arg1, Arg2 arg2)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
	}

	virtual void exec()
	{
		(m_obj->*m_m)(m_arg0, m_arg1, m_arg2);
	}

private:
	T *m_obj;
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
};

template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
class SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3)> : public SyncMethod {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1,Arg2,Arg3);

	SyncMethod_(T *obj, FunctionType m)
		: m_obj(obj)
		, m_m(m)
	{}

	void call(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
	}

	virtual void exec()
	{
		(m_obj->*m_m)(m_arg0, m_arg1, m_arg2, m_arg3);
	}

private:
	T *m_obj;
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
};


template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3,Arg4)> : public SyncMethod {
public:
	typedef Rt (T::*FunctionType)(Arg0,Arg1,Arg2,Arg3,Arg4);

	SyncMethod_(T *obj, FunctionType m)
		: m_obj(obj)
		, m_m(m)
	{}

	void call(Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
	{
		m_arg0 = arg0;
		m_arg1 = arg1;
		m_arg2 = arg2;
		m_arg3 = arg3;
		m_arg4 = arg4;
	}

	virtual void exec()
	{
		(m_obj->*m_m)(m_arg0, m_arg1, m_arg2, m_arg3, m_arg4);
	}

private:
	T *m_obj;
	FunctionType m_m;
	typename remove_const_reference<Arg0>::type m_arg0;
	typename remove_const_reference<Arg1>::type m_arg1;
	typename remove_const_reference<Arg2>::type m_arg2;
	typename remove_const_reference<Arg3>::type m_arg3;
	typename remove_const_reference<Arg4>::type m_arg4;
};


class RenderData
{
public:
	RenderData() : m_ref(), m_syncFrame(-1) {}
	virtual ~RenderData() {}

	int incref() { return m_ref.incref(); }
	int decref()
	{
		int result = m_ref.decref();
		if (result == 0) {
			deleteThis();
		}
		return result;
	}
	int getref() { return m_ref.getref(); }

	virtual void deleteThis()
	{
		g_renderQueue->addDeferredDeleteResource(this);
	}

	virtual void reallyDelete() { delete this; }

	void checkedSync()
	{
		if (m_syncFrame == g_renderSystem->getFrameNum())
			return;

//		queueCmd0(m_backend, &RenderBackend::sync).call();
		m_syncFrame = g_renderSystem->getFrameNum();
	}

	//template <class Signature>
	//void queCommand(Signature m)
	//{
	//	SyncMethod *sm = SyncMethod_(m);
	//}

	template <typename Rt, typename T>
	static SyncMethod_<Rt (T::*)()> &queueCmd0(T *obj, Rt (T::*method)())
	{
		typedef SyncMethod_<Rt (T::*)()> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(obj, method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0>
	static SyncMethod_<Rt (T::*)(Arg0)> &queueCmd1(T *obj, Rt (T::*method)(Arg0))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(obj, method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1)> &queueCmd2(T *obj, Rt (T::*method)(Arg0,Arg1))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(obj, method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2)> &queueCmd3(T *obj, Rt (T::*method)(Arg0,Arg1,Arg2))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(obj, method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3)> &queueCmd4(T *obj, Rt (T::*method)(Arg0,Arg1,Arg2,Arg3))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(obj, method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3,Arg4)> &queueCmd5(T *obj, Rt (T::*method)(Arg0,Arg1,Arg2,Arg3,Arg4))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3,Arg4)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(obj, method);
		return *result;
	}

protected:
	AtomicInt m_ref;
	int m_syncFrame;
};



class RenderCommand
{
public:
	virtual ~RenderCommand() {}
	virtual void exec() = 0;
};

struct CommandBuf
{
	enum {
		MaxBufSize = 64 * 1024
	};

	int bufSize;
	byte_t buf[MaxBufSize];
};

AX_END_NAMESPACE

#endif // AX_COMMANDBUF_H
