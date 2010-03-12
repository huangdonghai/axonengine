#ifndef AX_COMMANDBUF_H
#define AX_COMMANDBUF_H

AX_BEGIN_NAMESPACE

class RenderResource
{
public:
	RenderResource();
	virtual ~RenderResource();

	virtual void deleteThis()
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
class tag : public RenderCommand \
{ \
public: \
	tag() {} \
	~tag() {} \
	virtual void exec() \

#define AX_QUEUE_RENDER_COMMAND_1(tag, t1, a1, v1, code)
class tag :public RenderCommand
{
	t1 a1;
public:
	tag(t1 _##a1) : a1(_##a1) {} \
	~tag() {} \
	virtual void exec()

#define AX_QUEUE_RENDER_COMMAND_2(tag, t1, a1, v1, t2, a2, v2, code)
#define AX_QUEUE_RENDER_COMMAND_3(tag, t1, a1, v1, t2, a2, v2, t3, a3, v3, code)


AX_END_NAMESPACE

#endif // AX_COMMANDBUF_H
