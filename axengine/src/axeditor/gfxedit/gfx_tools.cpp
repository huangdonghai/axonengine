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
	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}

	m_gfxAgent = create();
	m_view->getContext()->setSelection(m_gfxAgent);

#if 0
	m_gfxAgent->setProperty("tree", m_gfxContext->getMapState()->treeFilename);

	if (m_gfxContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}

	m_gfxAgent->bindToGame();
#endif

	GroupHis* grouphis = new GroupHis(m_context, "Create GfxObject");

	AgentList agentlist;
	agentlist.push_back(m_gfxAgent);
	UndeleteHis* undelhis = new UndeleteHis(m_context, "Create GfxObject", agentlist);

	History* selhis = m_view->getContext()->setSelectionHistoried(agentlist);

	grouphis->append(undelhis);
	grouphis->append(selhis);

	m_context->addHistory(grouphis);

	doDrag(x, y, flags, pressure);
}

void GfxObjectCreationTool::doDrag(int x, int y, int flags, float pressure)
{
	if (!m_gfxAgent) {
		return;
	}

	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}
#if 0
	if (m_gfxContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}
#endif
	m_gfxAgent->setOrigin(from);
}

void GfxObjectCreationTool::doMove(int x, int y)
{

}

void GfxObjectCreationTool::doRelease(int x, int y)
{
	if (!m_gfxAgent) {
		return;
	}

	m_gfxAgent = nullptr;
}

void GfxObjectCreationTool::doRender(const RenderCamera& camera)
{

}

//--------------------------------------------------------------------------
ParticleCreationTool::ParticleCreationTool(GfxContext *ctx)
	: GfxObjectCreationTool(ctx)
{

}

ParticleCreationTool::~ParticleCreationTool()
{

}

GfxAgent * ParticleCreationTool::create()
{
	return new GfxAgent(m_gfxContext, GfxObject::kParticleEmitter);
}

AX_END_NAMESPACE
