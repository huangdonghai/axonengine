#include "gfx_local.h"

namespace Axon { namespace Editor {

	GfxTool::GfxTool( GfxContext *ctx )
		: Tool(ctx)
	{
		m_gfxContext = ctx;
	}

	GfxTool::~GfxTool()
	{

	}

}} // namespace Axon::Editor
