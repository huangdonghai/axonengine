#ifndef AX_COMMANDBUF_H
#define AX_COMMANDBUF_H

AX_BEGIN_NAMESPACE

class RenderInterface
{
public:
	enum TextureCreationFlag {
		TCF_NoMipmap = 1,
		TCF_NoDownsample = 2,
		TCF_RenderTarget = 4
	};

	enum Hint {
		Hint_Static, Hint_Dynamic
	};

	// new interface
	virtual handle_t createTexture2D(TexFormat format, int width, int height, int flags = 0);
	virtual void uploadTexture(handle_t htex, int level, void *pixels, TexFormat format = TexFormat::AUTO);
	virtual void uploadSubTexture(handle_t htex, const Rect &rect, const void *pixels, TexFormat format = TexFormat::AUTO);
	virtual void generateMipmap(handle_t htex);
	virtual void deleteTexture2D(handle_t htex);

	virtual handle_t createVertexBuffer(size_t datasize, Hint hint);
	virtual void *lockVertexBuffer(handle_t hvb);
	virtual void unlockVertexBuffer(handle_t hvb);
	virtual void deleteVertexBuffer(handle_t hvb);
	virtual handle_t createIndexBuffer(size_t datasize, Hint hint);
	virtual void *lockIndexBuffer(handle_t hib);
	virtual void unlockIndexBuffer(handle_t hib);
	virtual void deleteIndexBuffer(handle_t hib);

	virtual handle_t createShader(const String &name);
	//	virtual int setCurrentTechnique(handle_t shader, Technique tech, MaterialBr *mtr);
	virtual void setCurrentPass(int pass);

	virtual void drawIndexedPrimitive();
	virtual void drawIndexedPrimitiveUP();
};

class RenderResource
{
public:
	RenderResource();
	virtual ~RenderResource();

private:
};

class SyncMethod
{
public:
	SyncMethod() { g_renderQueue->addDeferredCommand(this); }
	virtual ~SyncMethod();
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

	SyncMethod_(FunctionType m)
		: m_obj(0)
		, m_m(m)
	{}

	void push(T *obj)
	{
		m_obj = obj;
	}

	virtual void exec()
	{
		m_obj->*m_m();
	}

private:
	T *m_obj;
	FunctionType m_m;
};

template <typename Rt, typename T, typename Arg0>
class SyncMethod_<Rt (T::*)(Arg0)> : public SyncMethod {
public:
	typedef Rt (T::*FunctionType)(Arg0);

	SyncMethod_(FunctionType m)
		: m_obj(0)
		, m_m(m)
	{}

	void call(T *obj, typename add_const_reference<Arg0>::type arg0)
	{
		m_obj = obj;
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

	SyncMethod_(FunctionType m)
		: m_obj(0)
		, m_m(m)
	{}

	void call(T *obj, typename add_const_reference<Arg0>::type arg0,
		typename add_const_reference<Arg1>::type arg1)
	{
		m_obj = obj;
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

	SyncMethod_(FunctionType m)
		: m_obj(0)
		, m_m(m)
	{}

	void call(T *obj, typename add_const_reference<Arg0>::type arg0,
		typename add_const_reference<Arg1>::type arg1,
		typename add_const_reference<Arg2>::type arg2)
	{
		m_obj = obj;
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

	SyncMethod_(FunctionType m)
		: m_obj(0)
		, m_m(m)
	{}

	void call(T *obj, typename add_const_reference<Arg0>::type arg0,
		typename add_const_reference<Arg1>::type arg1,
		typename add_const_reference<Arg2>::type arg2,
		typename add_const_reference<Arg3>::type arg3)
	{
		m_obj = obj;
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

	SyncMethod_(FunctionType m)
		: m_obj(0)
		, m_m(m)
	{}

	void call(T *obj, typename add_const_reference<Arg0>::type arg0,
		typename add_const_reference<Arg1>::type arg1,
		typename add_const_reference<Arg2>::type arg2,
		typename add_const_reference<Arg3>::type arg3,
		typename add_const_reference<Arg4>::type arg4)
	{
		m_obj = obj;
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
	RenderData() {}
	virtual ~RenderData() {}

	int incref() { return m_ref.incref(); }
	int decref()
	{
		int result = m_ref.decref();
		if (result == 0) {
			g_renderQueue->addDeferredDeleteResource(this);
		}
		return result;
	}
	int getref() { return m_ref.getref(); }

	virtual void deleteThis() { delete this; }

	//template <class Signature>
	//void queCommand(Signature m)
	//{
	//	SyncMethod *sm = SyncMethod_(m);
	//}

	template <typename Rt, typename T>
	static SyncMethod_<Rt (T::*)()> &queueCmd0(Rt (T::*method)())
	{
		typedef SyncMethod_<Rt (T::*)()> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0>
	static SyncMethod_<Rt (T::*)(Arg0)> &queueCmd1(Rt (T::*method)(Arg0))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1)> &queueCmd2(Rt (T::*method)(Arg0,Arg1))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2)> &queueCmd3(Rt (T::*method)(Arg0,Arg1,Arg2))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3)> &queueCmd4(Rt (T::*method)(Arg0,Arg1,Arg2,Arg3))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(method);
		return *result;
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	static SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3,Arg4)> &queueCmd5(Rt (T::*method)(Arg0,Arg1,Arg2,Arg3,Arg4))
	{
		typedef SyncMethod_<Rt (T::*)(Arg0,Arg1,Arg2,Arg3,Arg4)> ResultType;
		ResultType *result = g_renderQueue->allocType<ResultType>(1);
		new (result) ResultType(method);
		return *result;
	}

private:
	AtomicInt m_ref;
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

#define AX_QUEUE_RENDER_COMMAND_0(tag, code)
#define AX_QUEUE_RENDER_COMMAND_1(tag, t1, a1, v1, code)
#define AX_QUEUE_RENDER_COMMAND_2(tag, t1, a1, v1, t2, a2, v2, code)
#define AX_QUEUE_RENDER_COMMAND_3(tag, t1, a1, v1, t2, a2, v2, t3, a3, v3, code)


AX_END_NAMESPACE

#endif // AX_COMMANDBUF_H
