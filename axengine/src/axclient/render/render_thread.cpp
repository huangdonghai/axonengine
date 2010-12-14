#include "../private.h"

AX_BEGIN_NAMESPACE

RenderThread::RenderThread()
{

}

RenderThread::~RenderThread()
{

}

Thread::RunningStatus RenderThread::doRun()
{
	g_apiWrap->runCommands();
	return Thread::RS_Continue;
}

AX_END_NAMESPACE
