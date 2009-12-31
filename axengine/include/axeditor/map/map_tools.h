/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_EDITOR_MAP_TOOL_H
#define AX_EDITOR_MAP_TOOL_H

AX_BEGIN_NAMESPACE

class MapTool : public Tool {
public:
	enum MapToolType {
		// terrain height map modification
		TerrainRaise = Tool::UserDefined, TerrainLower, TerrainLevel, TerrainSmooth, TerrainGrab,
		TerrainPush, TerrainErode, TerrainRamp,

		// terrain paint
		TerrainPaint, TerrainErase,

		// terrain active area
		TerrainPaintActive,TerrainEraseActive,

		// grass / tree
		GrassPaint, GrassErase, TreePaint, TreeErase,

		// road / river
		Road, River,

		// entities
		CreateStatic, CreateBrush, CreateEntity, CreateTree,

	};

	MapTool(MapContext *ctx);

protected:
	MapContext *m_mapContext;
};

//--------------------------------------------------------------------------
// class TerrainRaiseTool, terrain raise tool
//--------------------------------------------------------------------------

class TerrainRaiseTool : public MapTool {
public:
	TerrainRaiseTool(MapContext *context);
	virtual ~TerrainRaiseTool();

	// Tool
	virtual void doBindView(View *view);
	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);
	virtual void doMove(int x, int y);
	virtual void doRelease(int x, int y);
	virtual void doRender(const RenderCamera &camera);

	virtual void onHistoryCreated(Action *action) { action->setMessage("Terrain Raise"); }

protected:
	virtual float getWeight(float x, float y) const;
	virtual void updatePrim(const Vector3 &from);

protected:
	MapTerrain *m_terrain;
	bool m_isValid;
	float m_brushRadius;
	float m_brushStrength;
	float m_brushSoftness;
	Vector2 m_center;
	LinePrim *m_cursor;
	Rect m_editedRect;

	MaterialPtr m_brushMat;
	GroupPrim *m_brushPrims;
};

//--------------------------------------------------------------------------
// class TerrainLowerTool, terrain lower tool
//--------------------------------------------------------------------------

class TerrainLowerTool : public TerrainRaiseTool {
public:
	TerrainLowerTool(MapContext *context);
	virtual ~TerrainLowerTool();

	virtual void onHistoryCreated(Action *action) { action->setMessage("Terrain Lower"); }

protected:
	virtual float getWeight(float x, float y) const;
};

//--------------------------------------------------------------------------
// class TerrainFlatTool, terrain level tool
//--------------------------------------------------------------------------

class TerrainFlatTool : public TerrainRaiseTool {
public:
	TerrainFlatTool(MapContext *context);
	virtual ~TerrainFlatTool();

	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);

	virtual void onHistoryCreated(Action *action) { action->setMessage("Terrain Flat"); }

protected:
	virtual float getWeight(float x, float y) const;

private:
	bool m_isJustPressed;
	float m_height;
};

//--------------------------------------------------------------------------
// class TerrainSmoothTool, terrain smooth tool
//--------------------------------------------------------------------------

class TerrainSmoothTool : public TerrainRaiseTool {
public:
	TerrainSmoothTool(MapContext *context);
	virtual ~TerrainSmoothTool();

	virtual void doDrag(int x, int y, int flags, float pressure);

	virtual void onHistoryCreated(Action *action) { action->setMessage("Terrain Smooth"); }
};


//--------------------------------------------------------------------------
// class TerrainGrabTool, terrain smooth tool
//--------------------------------------------------------------------------

class TerrainGrabTool : public TerrainRaiseTool {
public:
	TerrainGrabTool(MapContext *context);
	virtual ~TerrainGrabTool();

	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);

	virtual void onHistoryCreated(Action *action) { action->setMessage("Terrain Grab"); }
private:
	bool m_isJustPressed;
	int m_mouseY;
	Image *m_baseHeight;
};

//--------------------------------------------------------------------------
// class TerrainPaintTool, terrain paint tool
//--------------------------------------------------------------------------

class TerrainPaintTool : public TerrainRaiseTool {
public:
	TerrainPaintTool(MapContext *context);
	virtual ~TerrainPaintTool();

	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);
	virtual void doRelease(int x, int y);

protected:
	MapLayerGen *m_layerGen;
	Image *m_oldPixel;
};

//--------------------------------------------------------------------------
// class TerrainEraseTool, terrain erase tool
//--------------------------------------------------------------------------

class TerrainEraseTool : public TerrainPaintTool {
public:
	TerrainEraseTool(MapContext *context) : TerrainPaintTool(context) {}
	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doRelease(int x, int y);
};


//--------------------------------------------------------------------------
// class CreateStaticTool
//--------------------------------------------------------------------------

class CreateStaticTool : public MapTool {
public:
	CreateStaticTool(MapContext *context);
	virtual ~CreateStaticTool();

	virtual void doBindView(View *view) {}
	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);
	virtual void doMove(int x, int y) {}
	virtual void doRelease(int x, int y);
	virtual void doRender(const RenderCamera &camera) {}

private:
	MapStatic *m_agent;
};

//--------------------------------------------------------------------------
// class CreateEntityTool
//--------------------------------------------------------------------------

class CreateEntityTool : public MapTool {
public:
	CreateEntityTool(MapContext *context);
	virtual ~CreateEntityTool();

	virtual void doBindView(View *view) {}
	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);
	virtual void doMove(int x, int y) {}
	virtual void doRelease(int x, int y);
	virtual void doRender(const RenderCamera &camera);

private:
	MapActor *m_agent;
};

//--------------------------------------------------------------------------
// class CreateTreeTool
//--------------------------------------------------------------------------

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
class CreateTreeTool : public MapTool {
public:
	CreateTreeTool(MapContext *context);
	virtual ~CreateTreeTool();

	virtual void doBindView(View *view) {}
	virtual void doPress(int x, int y, int flags, float pressure);
	virtual void doDrag(int x, int y, int flags, float pressure);
	virtual void doMove(int x, int y) {}
	virtual void doRelease(int x, int y);
	virtual void doRender(const RenderCamera &camera) {}

private:
	MapSpeedTree *m_agent;
};
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

AX_END_NAMESPACE

#endif
