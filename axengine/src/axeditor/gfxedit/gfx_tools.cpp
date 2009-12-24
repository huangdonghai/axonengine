#include "gfx_local.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
GfxTool::GfxTool(GfxContext *ctx)
	: Tool(ctx)
{
	m_gfxContext = ctx;
}

GfxTool::~GfxTool()
{

}

//--------------------------------------------------------------------------

GfxObjectCreationTool::GfxObjectCreationTool(GfxContext *ctx)
	: GfxTool(ctx)
	, m_gfxAgent(0)
{

}

GfxObjectCreationTool::~GfxObjectCreationTool()
{

}

void GfxObjectCreationTool::doPress(int x, int y, int flags, float pressure)
{

}

void GfxObjectCreationTool::doDrag(int x, int y, int flags, float pressure)
{

}

void GfxObjectCreationTool::doMove(int x, int y)
{

}

void GfxObjectCreationTool::doRelease(int x, int y)
{

}

void GfxObjectCreationTool::doRender(const RenderCamera& camera)
{

}
AX_END_NAMESPACE
