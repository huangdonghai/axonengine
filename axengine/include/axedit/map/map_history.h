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
		MapHistory(const std::string &msg);
	};

	//--------------------------------------------------------------------------
	// class HeightmapHis, for terrain heightmap editing
	//--------------------------------------------------------------------------

	class HeightmapHis : public MapHistory {
	public:
		HeightmapHis(const std::string &msg, const Rect &rect, Image *olddata, Image *newdata, MapTerrain *terrain);
		virtual ~HeightmapHis();

		// implement Action
		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable();
		virtual std::string getName();
		virtual int getMemoryUsed();

	private:
		Rect m_tilerect;
		Image *m_oldData;
		Image *m_newData;
		MapTerrain *m_terrain;
	};

	//--------------------------------------------------------------------------
	// class TerrainMaterialDefHis, for terrain material def editing
	//--------------------------------------------------------------------------

	class TerrainMaterialDefHis : public MapHistory {
	public:
		TerrainMaterialDefHis(MapMaterialDef *old, MapMaterialDef *newdata, MapTerrain *terrain);
		virtual ~TerrainMaterialDefHis();

		// implement Action
		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable();
		virtual std::string getName();
		virtual int getMemoryUsed();

	private:
		MapMaterialDef *m_olddata;
		MapMaterialDef *m_newdata;
		MapTerrain *m_terrain;
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
		TerrainPaintHis(const std::string &msg, const Rect &rect, int layerId, Image *olddata, Image *newdata);
		virtual ~TerrainPaintHis();

		virtual void doIt();
		virtual void undo();
		virtual bool isUndoable();
		virtual std::string getName();
		virtual int getMemoryUsed();

	private:
		Rect m_pixelrect;
		int m_layerId;
		Image *m_olddata;
		Image *m_newdata;
	};

AX_END_NAMESPACE

#endif // end guardian

