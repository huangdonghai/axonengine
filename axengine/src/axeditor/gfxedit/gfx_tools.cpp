#include "gfx_local.h"

AX_BEGIN_NAMESPACE

	GfxTool::GfxTool( GfxContext *ctx )
		: Tool(ctx)
	{
		m_gfxContext = ctx;
	}

	GfxTool::~GfxTool()
	{

	}

AX_END_NAMESPACE
