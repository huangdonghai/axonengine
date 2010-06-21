#ifndef AX_RENDER_THREAD_H
#define AX_RENDER_THREAD_H

AX_BEGIN_NAMESPACE

class RenderThread : public Thread
{
public:
	RenderThread();
	~RenderThread();

	// implement thread run
	virtual void doRun();		// work entry

protected:

private:
};

AX_END_NAMESPACE

#endif // AX_RENDER_THREAD_H
