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
	SyncMethod_<Rt (T::*)()> &queCommand(Rt (T::*method)())
	{
	}

	template <typename Rt, typename T, typename Arg0>
	SyncMethod_<Rt (T::*)(Arg0)> &queCommand1(Rt (T::*method)(Arg0))
	{
	}

	template <typename Rt, typename T, typename Arg0, typename Arg1>
	void queCommand4(T *obj, Rt (T::*method)(const Arg0 &, const Arg1 &), const Arg0 &arg0, const Arg1 &arg1)
	{
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
