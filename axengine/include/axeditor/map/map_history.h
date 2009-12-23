/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_MAP_HISTORY_H
#define AX_EDITOR_MAP_HISTORY_H

AX_BEGIN_NAMESPACE

	class AX_API MapHistory : public History {
	public:
		MapHistory();
		MapHistory(const String& msg);
	};

	//--------------------------------------------------------------------------
	// class HeightmapHis, for terrain heightmap editing
	//--------------------------------------------------------------------------

	class HeightmapHis : public MapHistory {
	public:
		HeightmapHis(const String& msg, const Rect& rect, Image* olddata, Image* newdata, MapTerrain* terrain);
		virtual ~HeightmapHis();

		// implement Action
		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable();
		virtual String getName();
		virtual int getMemoryUsed();

	private:
		Rect m_tilerect;
		Image* m_oldData;
		Image* m_newData;
		MapTerrain* m_terrain;
	};

	//--------------------------------------------------------------------------
	// class TerrainMaterialDefHis, for terrain material def editing
	//--------------------------------------------------------------------------

	class TerrainMaterialDefHis : public MapHistory {
	public:
		TerrainMaterialDefHis(Axon::Map::MaterialDef* old, Axon::Map::MaterialDef* newdata, MapTerrain* terrain);
		virtual ~TerrainMaterialDefHis();

		// implement Action
		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable();
		virtual String getName();
		virtual int getMemoryUsed();

	private:
		Axon::Map::MaterialDef* m_olddata;
		Axon::Map::MaterialDef* m_newdata;
		MapTerrain* m_terrain;
	};

	//--------------------------------------------------------------------------
	// class TerrainSurfaceGenHis
	//--------------------------------------------------------------------------

	class TerrainSurfaceGenHis : public History {
	public:
	private:
	};

	//--------------------------------------------------------------------------
	// class TerrainPaintHis
	//--------------------------------------------------------------------------

	class TerrainPaintHis : public MapHistory {
	public:
		TerrainPaintHis(const String& msg, const Rect& rect, int layerId, Image* olddata, Image* newdata);
		virtual ~TerrainPaintHis();

		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable();
		virtual String getName();
		virtual int getMemoryUsed();

	private:
		Rect m_pixelrect;
		int m_layerId;
		Image* m_olddata;
		Image* m_newdata;
	};

	//--------------------------------------------------------------------------
	// class PaintGrassHis
	//--------------------------------------------------------------------------

	class PaintGrassHis : public MapHistory {
	public:
		PaintGrassHis(const String& msg, const Rect& rect, byte_t* newData, byte_t* oldData);
		virtual ~PaintGrassHis();

		virtual void doIt();
		virtual void undo();
		virtual String getName() { return "PaintGrassHis"; }
		virtual bool isUndoable() { return true; }
		virtual int getMemoryUsed();

	private:
		Rect m_rect;
		byte_t* m_newData;
		byte_t* m_oldData;
	};

AX_END_NAMESPACE

#endif // end guardian

