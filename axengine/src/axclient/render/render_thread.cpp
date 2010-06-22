#include "../private.h"

AX_BEGIN_NAMESPACE

RenderThread::RenderThread()
{

}

RenderThread::~RenderThread()
{

}

void RenderThread::doRun()
{
	while (1) {
		g_apiWrap->runCommands();
	}
}

AX_END_NAMESPACE
