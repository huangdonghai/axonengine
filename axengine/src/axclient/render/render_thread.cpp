#include "../private.h"

AX_BEGIN_NAMESPACE

RenderThread::RenderThread()
{

}

RenderThread::~RenderThread()
{

}

void RenderThread::runFrame(bool isInThread)
{
}

void RenderThread::doRun()
{
	while (1) runFrame(true);
}

AX_END_NAMESPACE
