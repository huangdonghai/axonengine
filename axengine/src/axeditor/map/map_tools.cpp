/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

AX_BEGIN_NAMESPACE

MapTool::MapTool(MapContext* ctx) : Tool(ctx)
{
	m_mapContext = ctx;
}

//------------------------------------------------------------------------------
// class TerrainRaiseTool
//------------------------------------------------------------------------------

TerrainRaiseTool::TerrainRaiseTool(MapContext* context) : MapTool(context) {
	m_cursor = nullptr;
	m_isValid = false;
	m_brushMat = Material::load("terrainbrush");
	m_brushPrims = nullptr;
}

TerrainRaiseTool::~TerrainRaiseTool() {
	SafeDelete(m_cursor);
	SafeDelete(m_brushPrims);
}

void TerrainRaiseTool::doBindView(View* view) {
	m_view = dynamic_cast<PerspectiveView*>(view);
	m_terrain = m_context->getTerrain();

	if (m_view != nullptr && m_terrain != nullptr)
		m_isValid = true;
}

void TerrainRaiseTool::doPress(int x, int y, int flags, float pressure) {
	if (!m_isValid)
		return;

	m_brushRadius = m_mapContext->getMapState()->terrainBrushSize;
	m_brushSoftness = m_mapContext->getMapState()->terrainBrushSoftness;
	m_brushStrength = m_mapContext->getMapState()->terrainBrushStrength;

	m_editedRect.clear();

	if (flags & InputEvent::ControlModifier)
		m_brushStrength = -m_brushStrength;

	doDrag(x, y, flags, pressure);
}

void TerrainRaiseTool::doDrag(int x, int y, int flags, float pressure) {
	if (!m_isValid)
		return;

	Vector3 from;

	if (!m_view->selectRegion(Rect(x, y, 1, 1), SelectPart::kTerrain, from)) {
		return;
	}

	updatePrim(from);

	float tm = m_terrain->getTileMeters();

	from.x /= tm;
	from.y /= tm;
	int tileradius = m_brushRadius / tm;

	Rect rect(from.x - tileradius, from.y - tileradius, tileradius * 2, tileradius * 2);
	rect.inflate(1, 1);
	rect &= m_terrain->getTileRect();
	m_editedRect |= rect;

	for (int j = rect.y; j < rect.yMax(); j++) {
		for (int i = rect.x; i < rect.xMax(); i ++) {
			float h = m_terrain->getHeight(i, j);
			h += getWeight(i * tm, j * tm) * m_brushRadius / 10.0f;
			m_terrain->setHeight(i, j, h);
		}
	}

	m_terrain->doHeightChanged(rect);
}

void TerrainRaiseTool::doMove(int x, int y) {
	if (!m_isValid)
		return;

	Vector3 from;
	if (!m_view->selectRegion(Rect(x, y, 1, 1), SelectPart::kTerrain, from)) {
		return;
	}

	updatePrim(from);
}

void TerrainRaiseTool::doRelease(int x, int y) {
	if (!m_isValid)
		return;

	// create undo action
	String msg = "Edit Height";
	Image* oldimage = m_terrain->copyOldHeight(m_editedRect);
	Image* newimage = m_terrain->copyHeight(m_editedRect);
	m_terrain->writeOldHeight(m_editedRect, newimage);
	HeightmapHis* action = new HeightmapHis(msg, m_editedRect, oldimage, newimage, m_terrain);

	onHistoryCreated(action);

	m_view->getContext()->addHistory(action);
}

void TerrainRaiseTool::doRender(const RenderCamera& camera) {
	if (!m_isValid)
		return;

	if (m_cursor != NULL)
		g_renderSystem->addToScene(m_cursor);

	if (m_brushPrims)
		g_renderSystem->addToScene(m_brushPrims);
}

float TerrainRaiseTool::getWeight(float x, float y) const {
	Vector2 pos(x, y);
	float dist = (pos - m_center).getLength();

	dist /= m_brushRadius;
	dist = 1.0f - dist;
	dist /= m_brushSoftness;
	dist = Math::saturate(dist);
	dist *= m_brushStrength;

	return dist;
}

void TerrainRaiseTool::updatePrim(const Vector3& from) {
	if (from.x == m_center.x && from.y == m_center.y)
		return;

	Vector3 to = from; to.z += 2.0f;
	float radius = m_mapContext->getMapState()->terrainBrushSize;

	m_center.x = from.x; m_center.y = from.y;
	SafeDelete(m_cursor);
	m_cursor = LinePrim::createLine(Primitive::HintDynamic, from, to, Rgba::Red);

	SafeDelete(m_brushPrims);
	Vector4 rect = m_terrain->getTerrainRect();
	m_brushPrims = new GroupPrim(Primitive::HintDynamic);
	Primitives prims = m_terrain->getPrimsByCircle(from.x, from.y, radius);
	for (Primitives::iterator it = prims.begin(); it != prims.end(); ++it) {
		RefPrim* ref = new RefPrim(Primitive::HintDynamic);
		ref->setRefered(*it);
		ref->setMaterial(m_brushMat.get());
		m_brushPrims->addPrimitive(ref, true);
	}
	Matrix4 matrix;
	matrix.setTranslate(-(from.x-radius), -(from.y-radius), 0);
	matrix.scale(0.5f/radius, 0.5f/radius, 1);
	TexGen texgen;
	texgen.type = SHADER::TexGen_vertex;
	texgen.transform = true;
	texgen.matrix = matrix;
#if 0
	m_brushMat->setBaseTcMatrix(matrix);
#else
	m_brushMat->setTexGen(SamplerType::Diffuse, texgen);
#endif
//		m_view->doUpdate();
}


//------------------------------------------------------------------------------
// class TerrainLowerTool, terrain lower tool
//------------------------------------------------------------------------------

TerrainLowerTool::TerrainLowerTool(MapContext* context) : TerrainRaiseTool(context)
{}

TerrainLowerTool::~TerrainLowerTool() {}

float TerrainLowerTool::getWeight(float x, float y) const {
	return -TerrainRaiseTool::getWeight(x, y);
}

//------------------------------------------------------------------------------
// class TerrainFlatTool, terrain level tool
//------------------------------------------------------------------------------

TerrainFlatTool::TerrainFlatTool(MapContext* context) : TerrainRaiseTool(context)
{}

TerrainFlatTool::~TerrainFlatTool() {}


void TerrainFlatTool::doPress(int x, int y, int flags, float pressure) {
	m_isJustPressed = true;

	return TerrainRaiseTool::doPress(x, y, flags, pressure);
}

void TerrainFlatTool::doDrag(int x, int y, int flags, float pressure) {
	Vector3 from;

	if (!m_view->selectRegion(Rect(x, y, 1, 1), SelectPart::kTerrain, from)) {
		return;
	}

	updatePrim(from);

	if (m_isJustPressed) {
		m_height = from.z;
		m_isJustPressed = false;
	}

	float tm = m_terrain->getTileMeters();

	from.x /= tm;
	from.y /= tm;
	int tileradius = m_brushRadius / tm;

	Rect rect(from.x - tileradius, from.y - tileradius, tileradius * 2, tileradius * 2);
	rect.inflate(1, 1);
	rect &= m_terrain->getTileRect();
	m_editedRect |= rect;

	for (int j = rect.y; j < rect.yMax(); j++) {
		for (int i = rect.x; i < rect.xMax(); i ++) {
			float weight = getWeight(i*tm, j*tm);
			if (weight <= 0)
				continue;

			m_terrain->setHeight(i, j, m_height);
		}
	}

	m_terrain->doHeightChanged(rect);
}

float TerrainFlatTool::getWeight(float x, float y) const {
	Vector2 pos(x, y);
	float dist = (pos - m_center).getLength();

	dist /= m_brushRadius;
	dist = 1.0f - dist;
	dist /= m_brushSoftness;

	if (dist >= 0.0f)
		dist = 1.0f;
	else
		dist = 0.0f;

	return dist;
}


//------------------------------------------------------------------------------
// class TerrainSmoothTool, terrain smooth tool
//------------------------------------------------------------------------------

TerrainSmoothTool::TerrainSmoothTool(MapContext* context) : TerrainRaiseTool(context)
{}

TerrainSmoothTool::~TerrainSmoothTool() {}

void TerrainSmoothTool::doDrag(int x, int y, int flags, float pressure) {
	if (!m_isValid)
		return;

	Vector3 from;

	if (!m_view->selectRegion(Rect(x, y, 1, 1), SelectPart::kTerrain, from)) {
		return;
	}

	updatePrim(from);

	float tm = m_terrain->getTileMeters();

	from.x /= tm;
	from.y /= tm;
	int tileradius = m_brushRadius / tm;

	Rect rect(from.x - tileradius, from.y - tileradius, tileradius * 2, tileradius * 2);
	rect.inflate(1, 1);
	rect &= m_terrain->getTileRect();
	m_editedRect |= rect;

	int smoothkernel = m_brushRadius / 16;
	if (smoothkernel < 1)
		smoothkernel = 1;

	for (int j = rect.y; j < rect.yMax(); j++) {
		for (int i = rect.x; i < rect.xMax(); i ++) {
			float avg = m_terrain->getHeight(i-smoothkernel, j) + m_terrain->getHeight(i+smoothkernel, j)
						+ m_terrain->getHeight(i, j-smoothkernel) + m_terrain->getHeight(i, j+smoothkernel);

			avg *= 0.25f;
			float h = m_terrain->getHeight(i, j);
			float frac = getWeight(i * tm, j * tm);
			m_terrain->setHeight(i, j, h * (1.0f - frac) + avg * frac);
		}
	}

	m_terrain->doHeightChanged(rect);
}

//------------------------------------------------------------------------------
// class TerrainGrabTool, terrain level tool
//------------------------------------------------------------------------------

TerrainGrabTool::TerrainGrabTool(MapContext* context) : TerrainRaiseTool(context) {
	m_isJustPressed = false;
	m_baseHeight = nullptr;
}

TerrainGrabTool::~TerrainGrabTool() {}

void TerrainGrabTool::doPress(int x, int y, int flags, float pressure) {
	m_isJustPressed = true;

	return TerrainRaiseTool::doPress(x, y, flags, pressure);
}

void TerrainGrabTool::doDrag(int x, int y, int flags, float pressure) {
	if (!m_isValid)
		return;

	float tm = m_terrain->getTileMeters();

	if (!m_isJustPressed) {
		if (!m_baseHeight) {
			return;
		}

		Rect rect = m_editedRect;
		for (int j = rect.y; j < rect.yMax(); j++) {
			for (int i = rect.x; i < rect.xMax(); i ++) {
				float h = m_terrain->getOldHeight(i, j) + (m_mouseY - y) * getWeight(i * tm, j * tm);
				m_terrain->setHeight(i, j, h);
			}
		}

		m_terrain->doHeightChanged(rect);
		return;
	}

	Vector3 from;

	if (!m_view->selectRegion(Rect(x, y, 1, 1), SelectPart::kTerrain, from)) {
		return;
	}

	updatePrim(from);

	from.x /= tm;
	from.y /= tm;
	int tileradius = m_brushRadius / tm;

	Rect rect(from.x - tileradius, from.y - tileradius, tileradius * 2, tileradius * 2);
	rect.inflate(1, 1);
	rect &= m_terrain->getTileRect();
	m_editedRect = rect;

	m_baseHeight = m_terrain->copyHeight(m_editedRect);
	m_mouseY = y;

	m_isJustPressed = false;
}

//--------------------------------------------------------------------------
// class TerrainPaintTool, terrain paint tool
//--------------------------------------------------------------------------

TerrainPaintTool::TerrainPaintTool(MapContext* context) : TerrainRaiseTool(context) {
	m_oldPixel = nullptr;
}

TerrainPaintTool::~TerrainPaintTool() {}

void TerrainPaintTool::doPress(int x, int y, int flags, float pressure) {
	if (!m_isValid)
		return;

	m_brushRadius = m_mapContext->getMapState()->terrainBrushSize;
	m_brushSoftness = m_mapContext->getMapState()->terrainBrushSoftness;
	m_brushStrength = m_mapContext->getMapState()->terrainBrushStrength;
	m_layerGen = m_terrain->getLayerGenById(m_mapContext->getMapState()->terrainCurLayerId);

	m_editedRect.clear();

	if (flags & InputEvent::ControlModifier)
		m_brushStrength = -m_brushStrength;

	SafeDelete(m_oldPixel);

	doDrag(x, y, flags, pressure);
}

void TerrainPaintTool::doDrag(int x, int y, int flags, float pressure) {
	if (!m_isValid || ! m_layerGen)
		return;

	Vector3 from;

	if (!m_view->selectRegion(Rect(x, y, 1, 1), SelectPart::kTerrain, from)) {
		return;
	}

	updatePrim(from);

	float tm = m_terrain->getTileMeters();
	float meterpixels = Map::TilePixels / tm;

	from.x *= meterpixels;
	from.y *= meterpixels;
	int pixelradius = m_brushRadius * meterpixels;

	Rect rect(from.x - pixelradius, from.y - pixelradius, pixelradius * 2, pixelradius * 2);
	rect.inflate(1, 1);
	rect &= m_terrain->getTileRect() * Map::TilePixels;

	if (!m_editedRect.contains(rect)) {
		Rect totalrect = m_editedRect | rect;
		Image* image = m_layerGen->copyAlpha(totalrect);

		if (m_oldPixel) {
			image->writeSubImage(m_oldPixel, Rect(0,0,m_editedRect.width,m_editedRect.height), m_editedRect.getMins() - totalrect.getMins());
			delete m_oldPixel;
		}

		m_oldPixel = image;
		m_editedRect = totalrect;
	}

	for (int j = rect.y; j < rect.yMax(); j++) {
		for (int i = rect.x; i < rect.xMax(); i ++) {
			float alpha = m_layerGen->getAlpha(i, j);
			alpha += getWeight(i / meterpixels, j / meterpixels);
			m_layerGen->setAlpha(i, j, alpha);
		}
	}

	m_terrain->doLayerPainted(rect);
}

void TerrainPaintTool::doRelease(int x, int y) {
	if (!m_isValid || ! m_layerGen)
		return;

	// create undo action
	String msg = "mapTerrain Paint";
	Image* newimage = m_layerGen->copyAlpha(m_editedRect);
	TerrainPaintHis* action = new TerrainPaintHis(msg, m_editedRect, m_layerGen->getLayerId(), m_oldPixel, newimage);

	m_oldPixel = nullptr;

	m_view->getContext()->addHistory(action);
}

//--------------------------------------------------------------------------
// class TerrainEraseTool, terrain erase tool
//--------------------------------------------------------------------------

void TerrainEraseTool::doPress( int x, int y, int flags, float pressure )
{
	if (!m_isValid)
		return;

	m_brushRadius = m_mapContext->getMapState()->terrainBrushSize;
	m_brushSoftness = m_mapContext->getMapState()->terrainBrushSoftness;
	m_brushStrength = -m_mapContext->getMapState()->terrainBrushStrength;
	m_layerGen = m_terrain->getLayerGenById(m_mapContext->getMapState()->terrainCurLayerId);

	m_editedRect.clear();

	if (flags & InputEvent::ControlModifier)
		m_brushStrength = -m_brushStrength;

	SafeDelete(m_oldPixel);

	doDrag(x, y, flags, pressure);
}

void TerrainEraseTool::doRelease( int x, int y )
{
	if (!m_isValid || ! m_layerGen)
		return;

	// create undo action
	String msg = "mapTerrain Erase";
	Image* newimage = m_layerGen->copyAlpha(m_editedRect);
	TerrainPaintHis* action = new TerrainPaintHis(msg, m_editedRect, m_layerGen->getLayerId(), m_oldPixel, newimage);

	m_oldPixel = nullptr;

	m_view->getContext()->addHistory(action);
}

//--------------------------------------------------------------------------
// class CreateStaticTool
//--------------------------------------------------------------------------

CreateStaticTool::CreateStaticTool(MapContext* context) : MapTool(context) {
	m_agent = nullptr;
}

CreateStaticTool::~CreateStaticTool() {
}

void CreateStaticTool::doPress(int x, int y, int flags, float pressure) {
	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}

	m_agent = new MapStatic();

	m_agent->setProperty("model", m_mapContext->getMapState()->staticModelName);

	if (m_mapContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}

	m_agent->setOrigin(from);

	m_agent->bindToGame();

	GroupHis* grouphis = new GroupHis(m_context, "Create MapStatic");

	AgentList agentlist;
	agentlist.push_back(m_agent);
	UndeleteHis* undelhis = new UndeleteHis(m_context, "Create MapStatic", agentlist);

	History* selhis = m_view->getContext()->setSelectionHistoried(agentlist);

	grouphis->append(undelhis);
	grouphis->append(selhis);

	m_context->addHistory(grouphis);

	doDrag(x, y, flags, pressure);
}

void CreateStaticTool::doDrag(int x, int y, int flags, float pressure) {
	if (!m_agent) {
		return;
	}

	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}

	if (m_mapContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}

	m_agent->setOrigin(from);
}

void CreateStaticTool::doRelease(int x, int y) {
	if (!m_agent) {
		return;
	}

	m_agent = nullptr;
}

//--------------------------------------------------------------------------
// class CreateEntityTool
//--------------------------------------------------------------------------

CreateEntityTool::CreateEntityTool(MapContext* context) : MapTool(context) {
	m_agent = nullptr;
}

CreateEntityTool::~CreateEntityTool() {}

void CreateEntityTool::doPress(int x, int y, int flags, float pressure) {
	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}

	m_agent = new MapActor(m_mapContext->getMapState()->entityClass);

	if (m_mapContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}

	m_agent->getGameEntity()->autoGenerateName();
	m_agent->setOrigin(from);

	m_agent->bindToGame();

	GroupHis* grouphis = new GroupHis(m_context, "Create Entity");

	AgentList actorlist;
	actorlist.push_back(m_agent);
	UndeleteHis* undelhis = new UndeleteHis(m_context, "Create Entity", actorlist);

	History* selhis = m_view->getContext()->setSelectionHistoried(actorlist);

	grouphis->append(undelhis);
	grouphis->append(selhis);

	m_context->addHistory(grouphis);
}

void CreateEntityTool::doDrag(int x, int y, int flags, float pressure) {
	if (!m_agent) {
		return;
	}

	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}

	if (m_mapContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}

	m_agent->setOrigin(from);
}

void CreateEntityTool::doRelease(int x, int y) {
	if (!m_agent) {
		return;
	}

	m_agent = nullptr;
}

void CreateEntityTool::doRender(const RenderCamera& camera) {

}

//--------------------------------------------------------------------------
// class CreateTreeTool
//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
CreateTreeTool::CreateTreeTool(MapContext* context) : MapTool(context) {
	m_agent = nullptr;
}

CreateTreeTool::~CreateTreeTool() {
}

void CreateTreeTool::doPress(int x, int y, int flags, float pressure) {
	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}

	m_agent = new MapSpeedTree();
	m_view->getContext()->setSelection(m_agent);

	m_agent->setProperty("tree", m_mapContext->getMapState()->treeFilename);

	if (m_mapContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}

	m_agent->bindToGame();

	GroupHis* grouphis = new GroupHis(m_context, "Create MapSpeedTree");

	AgentList actorlist;
	actorlist.push_back(m_agent);
	UndeleteHis* undelhis = new UndeleteHis(m_context, "Create MapSpeedTree", actorlist);

	History* selhis = m_view->getContext()->setSelectionHistoried(actorlist);

	grouphis->append(undelhis);
	grouphis->append(selhis);

	m_context->addHistory(grouphis);

	doDrag(x, y, flags, pressure);
}

void CreateTreeTool::doDrag(int x, int y, int flags, float pressure) {
	if (!m_agent) {
		return;
	}

	Vector3 from;

	if (!m_view->traceWorld(x, y, from, SelectPart::All)) {
		return;
	}

	if (m_mapContext->getMapState()->isSnapToGrid) {
		from = Internal::snap(from, m_mapContext->getMapState()->snapToGrid);
	}

	m_agent->setOrigin(from);
}

void CreateTreeTool::doRelease(int x, int y) {
	if (!m_agent) {
		return;
	}

	m_agent = nullptr;
}
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

AX_END_NAMESPACE

