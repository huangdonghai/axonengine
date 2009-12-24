/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

AX_BEGIN_NAMESPACE
		
	//--------------------------------------------------------------------------
	// class HeightmapHis, for terrain heightmap editing
	//--------------------------------------------------------------------------

	HeightmapHis::HeightmapHis(const String& msg, const Rect& rect, Image* olddata, Image* newdata, MapTerrain* terrain)
		: MapHistory(msg)
	{
		m_tilerect = rect;
		m_oldData = olddata;
		m_newData = newdata;
		m_terrain = terrain;
	}

	HeightmapHis::~HeightmapHis() {
		SafeDelete(m_oldData);
		SafeDelete(m_newData);
	}

	void HeightmapHis::doIt() {
		m_terrain->writeHeight(m_tilerect, m_newData);
		m_terrain->writeOldHeight(m_tilerect, m_newData);
		m_terrain->doHeightChanged(m_tilerect);
	}

	void HeightmapHis::undo() {
		m_terrain->writeHeight(m_tilerect, m_oldData);
		m_terrain->writeOldHeight(m_tilerect, m_oldData);
		m_terrain->doHeightChanged(m_tilerect);
	}

	bool HeightmapHis::isUndoable() {
		return true;
	}

	String HeightmapHis::getName() { return "HeightmapHis";}

	int HeightmapHis::getMemoryUsed() {
		return m_newData->getTotalDataSize() * 2 + sizeof(HeightmapHis);
	}

	//--------------------------------------------------------------------------
	// class TerrainMaterialDefHis, for terrain material def editing
	//--------------------------------------------------------------------------

	TerrainMaterialDefHis::TerrainMaterialDefHis(Map::MaterialDef* old, Map::MaterialDef* newdata, MapTerrain* terrain)
		: MapHistory("MaterialDef Edit")
	{
		m_olddata = old;
		m_newdata = newdata;
		m_terrain = terrain;
	}
	TerrainMaterialDefHis::~TerrainMaterialDefHis() {
		SafeDelete(m_olddata);
		SafeDelete(m_newdata);
	}

	// implement Action
	void TerrainMaterialDefHis::doIt() {
		m_terrain->setMaterialDef(m_newdata->clone(), false);
	}

	void TerrainMaterialDefHis::undo() {
		m_terrain->setMaterialDef(m_olddata->clone(), false);
	}

	bool TerrainMaterialDefHis::isUndoable() {
		return true;
	}

	String TerrainMaterialDefHis::getName() {
		return "TerrainMaterialDefHis";
	}

	int TerrainMaterialDefHis::getMemoryUsed() {
		return sizeof(Map::MaterialDef) * 2;
	}

	//--------------------------------------------------------------------------
	// class TerrainPaintHis
	//--------------------------------------------------------------------------

	TerrainPaintHis::TerrainPaintHis(const String& msg, const Rect& rect, int layerId, Image* olddata, Image* newdata)
		: MapHistory(msg)
	{
		m_pixelrect = rect;
		m_layerId = layerId;
		m_olddata = olddata;
		m_newdata = newdata;
	}

	TerrainPaintHis::~TerrainPaintHis() {
		delete m_olddata;
		delete m_newdata;
	}

	void TerrainPaintHis::doIt() {
		MapTerrain* terrain = m_context->getTerrain();
		if (!terrain)
			return;

		Map::LayerGen* lg = terrain->getLayerGenById(m_layerId);
		if (!lg)
			return;

		lg->writeAlpha(m_pixelrect, m_newdata);

		terrain->doLayerPainted(m_pixelrect);
	}

	void TerrainPaintHis::undo() {
		MapTerrain* terrain = m_context->getTerrain();
		if (!terrain)
			return;

		Map::LayerGen* lg = terrain->getLayerGenById(m_layerId);
		if (!lg)
			return;

		lg->writeAlpha(m_pixelrect, m_olddata);

		terrain->doLayerPainted(m_pixelrect);
	}

	bool TerrainPaintHis::isUndoable() {
		return true;
	}

	String TerrainPaintHis::getName() {
		return "TerrainPaintHis";
	}

	int TerrainPaintHis::getMemoryUsed() {
		return m_newdata->getTotalDataSize() + m_olddata->getTotalDataSize() + sizeof(TerrainPaintHis);
	}



	MapHistory::MapHistory() : History(g_mapContext)
	{

	}

	MapHistory::MapHistory( const String& msg ) : History(g_mapContext, msg)
	{

	}

AX_END_NAMESPACE

