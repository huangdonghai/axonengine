/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_PRIMITIVE_H
#define AX_RENDER_PRIMITIVE_H

AX_BEGIN_NAMESPACE

class VertexObject;
class InstanceObject;
class IndexObject;

//--------------------------------------------------------------------------

// virtual base, can't create an instance for this
class AX_API Primitive
{
public:
	enum Type {
		NoneType = 0,		// for error checks
		PointType,
		LineType,
		MeshType,			// raw mesh
		TextType,
		GuiType,
		ChunkType,
		GroupType,			// grouped primitive, maybe for gui system
		ReferenceType,		// a reference to another primitive, but can use different entity and material etc...
		InstancingType		// geometry instancing type, instancing a primitive many time with different parameter
	};

	enum Hint {
		HintStatic,				// will alloc in video memory, never change
		HintDynamic,			// 
		HintFrame,				// only draw in one frame, will auto deleted when render driver cached it
	};

	Primitive(Hint hint);
	virtual ~Primitive() = 0;

	inline Hint getHint() const { return m_hint; }
#if 0
	inline int getCachedFrame() const { return m_cachedFrame; }
	inline void setCachedFrame(int frame) { m_cachedFrame = frame; }
#endif
	inline Type getType() const { return m_type; }

	inline bool isDirty() const { return m_isDirty; }
	inline bool isVertexBufferDirty() const { return m_isVertexBufferDirty; }
	inline bool isIndexBufferDirty() const { return m_isIndexBufferDirty; }
	inline void clearDirty() { m_isDirty = m_isVertexBufferDirty = m_isIndexBufferDirty = false; }

	inline bool isWorldSpace() const { return m_isWorldSpace; }
	inline void setWorldSpace(bool val) { m_isWorldSpace = val; }

	inline bool isMesh() const { return m_type == MeshType; }

	inline void setMatrix(const Matrix4 &matrix) { m_isMatrixSet = true; m_matrix = matrix; }
	inline bool isMatrixSet() const { return m_isMatrixSet; }
	inline Matrix4 getMatrix() const { return m_matrix; }
	inline void disableMatrix() { m_isMatrixSet = false; }

	inline Material *getMaterial() const;
	inline void setMaterial(Material *material);
	inline void setLightMap(Texture *lm);
	inline Texture *getLightMap() const;

	inline int getActivedIndexes() const { return m_activedIndexes; }
	inline void setActivedIndexes(int val) { m_activedIndexes = val; }

	void interactionChain(Interaction *last, int chainId);
	Interaction *getHeadInteraction() const { return m_headInteraction; }
	int getNumChainedInteractions() const { return m_numChainedInteractions; }

	virtual void draw(Technique tech) = 0;
	virtual void sync() = 0;

protected:
	const Hint m_hint;
	bool m_isDirty;		// dirty
	bool m_isVertexBufferDirty;
	bool m_isIndexBufferDirty;
	bool m_isWorldSpace; // primitive already in world space, so don't need model transform

#if 0
	int m_cachedId;		// used by render driver
	int m_cachedFrame;
#endif
	int m_syncFrame;

	Type m_type;
	Material *m_material;
	Texture *m_lightMap;

	bool m_isMatrixSet;
	Matrix4 m_matrix;

	int m_activedIndexes;

	int m_chainId;
	Interaction *m_headInteraction;
	int m_numChainedInteractions;

	// overload parameter
	InstanceObject *m_overloadInstanceObject;
	Material *m_overloadMaterial;
	IndexObject *m_overloadIndexObject;

	// geometry data
	VertexObject *m_vertexObject;
	IndexObject *m_indexObject;
};

inline void Primitive::interactionChain(Interaction *last, int chainId)
{
	if (m_chainId != chainId) {
		m_headInteraction = last;
		m_chainId = chainId;
		m_numChainedInteractions = 1;
		return;
	}

	last->primNext = m_headInteraction;
	m_headInteraction = last;
	m_numChainedInteractions++;
}

inline Material *Primitive::getMaterial() const
{
	return m_material;
}

inline void Primitive::setMaterial(Material *material)
{
	m_material = material;
}

inline void Primitive::setLightMap(Texture *lm)
{
	m_lightMap = lm;
}

inline Texture *Primitive::getLightMap() const
{
	return m_lightMap;
}


typedef std::vector<Primitive*> Primitives;

//--------------------------------------------------------------------------
// class PointPrim
//--------------------------------------------------------------------------

class AX_API PointPrim : public Primitive
{
public:
	PointPrim(Hint hint);
	virtual ~PointPrim();

	void initialize(int num_points);

	int getNumPoints() const;
	const DebugVertex *getPointsPointer() const;
	DebugVertex *lock();
	void unlock();

	// pointCount == -1, from offset to end
	void setDrawOffsetCount(int pointOffset = 0, int pointCount = -1);
	int getDrawOffset();
	int getDrawCount();

	void setPointSize(float point_size);
	float getPointSize() const;

	virtual void draw(Technique tech) {}
	virtual void sync() {}

private:
	int m_numPoints;
	DebugVertex *m_points;
	float m_pointSize;

	int m_pointDrawOffset;
	int m_pointDrawCount;
};

//--------------------------------------------------------------------------
// class LinePrim
//--------------------------------------------------------------------------

class AX_API LinePrim : public Primitive
{
public:
	LinePrim(Hint hint);
	virtual ~LinePrim();

	void init(int numverts, int numidxes);
	void clear();

	int getNumVertexes() const;
	const DebugVertex *getVertexesPointer() const;
	DebugVertex *lockVertexes();
	void unlockVertexes();

	int getNumIndexes() const;
	const ushort_t *getIndexPointer() const;
	ushort_t *lockIndexes();
	void unlockIndexes();

	// helper
	ushort_t getIndex(int order) const;
	const DebugVertex &getVertex(int order) const;

	void lock();
	void setIndex(int order, int index);
	void setVertex(int order, const DebugVertex &vert);
	DebugVertex &getVertexRef(int order);
	void unlock();

	void setLineWidth(float line_width);
	float getLineWidth() const;

	virtual void draw(Technique tech);
	virtual void sync();

	// helper static create
	static LinePrim *createAxis(Hint hint, float line_length);
	static LinePrim *createAxis(Hint hint, const Vector3 &origin, const Matrix3 &axis, float line_length);
	static LinePrim *createScreenRect(Hint hint, const Rect &rect, const Rgba &color);
	static LinePrim *createLine(Hint hint, const Vector3 &from, const Vector3 &to, const Rgba &color);
	static LinePrim *createWorldBoundingBox(Hint hint, const BoundingBox &bbox, const Rgba &color);

	static bool setupLine(LinePrim*& line, const Vector3 &from, const Vector3 &to, const Rgba &color);
	static bool setupAxis(LinePrim*& line, const Vector3 &origin, const Matrix3 &axis, float length, Rgba xcolor, Rgba ycolor, Rgba zcolor);
	static bool setupBoundingBox(LinePrim*& line, const Vector3 &origin, const Matrix3 &axis, const BoundingBox &inbbox, float scale=1.0f, Hint hint = Primitive::HintDynamic);
	static bool setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided);
	static bool setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided ,Hint hint);
	static bool setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided, const Plane &plane);
	static bool setupCircle(LinePrim*& line, const Vector3 &origin, const Vector3 &p0, const Vector3 &p1, const Rgba &color, int subdivided, bool clipplane, const Plane &plane,Hint hint = Primitive::HintDynamic);
	static bool setupScreenRect(LinePrim*& line, const Rect &rect, const Rgba &color);

private:
	int m_numVertexes;
	DebugVertex *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;
	float m_lineWidth;
};

//--------------------------------------------------------------------------
// class MeshPrim 
//--------------------------------------------------------------------------

class AX_API MeshPrim : public Primitive
{
public:
	MeshPrim(Hint hint);
	virtual ~MeshPrim();

	void init(int numverts, int numidxes);
	void clear();

	int getNumVertexes() const;
	const MeshVertex *getVertexesPointer() const;
	MeshVertex *lockVertexes();
	void unlockVertexes();

	const ushort_t *getIndexPointer() const;
	int getNumIndexes() const;
	ushort_t *lockIndexes();
	void unlockIndexes();

	LinePrim *getTangentLine(float len) const;
	LinePrim *getNormalLine(float len) const;

	void computeTangentSpace();
	void computeTangentSpaceSlow();

	bool isStriped() const { return m_isStriped; }
	void setStriped(bool val) { m_isStriped = val; }

	virtual void draw(Technique tech);
	virtual void sync();

	// static helper function
	static MeshPrim *createScreenQuad(Hint hint, const Rect &rect, const Rgba &color, Material *material=nullptr, const Vector4 &st = Vector4(0,0,1,1));
	static MeshPrim *createQuad(Hint hint, const Vector3 &p0, const Vector3 &p1);
	static MeshPrim *createAABB(Hint hint, const BoundingBox &box);
	static MeshPrim *createSphere(Hint hint, const Vector3 &origin, float radius, int subdivided);
	static MeshPrim *createHalfSphere(Hint hint, const Vector3 &origin, float radius, int subdivided, int side);
	static MeshPrim *createCylinder(Hint hint, const Vector3 &p0, const Vector3 &p1, float radius);

	static bool setupFan(MeshPrim*& mesh, const Vector3 &center, const Vector3 &v0, const Vector3 &v1, float start, float end, Rgba color, int subdivided, Material *material = nullptr);
	static bool setupQuad(MeshPrim*& mesh, const Vector3 &p0, const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, const Rgba &color, Material *material = nullptr);
	static bool setupPolygon(MeshPrim*& mesh, int numverts, const Vector3 *verts, Rgba color, Material *material = nullptr);
	static bool setupCone(MeshPrim*& mesh, const Vector3 &center, float radius, const Vector3 &top, const Rgba &color, int subdivided = 8);
	static bool setupBox(MeshPrim*& mesh, const BoundingBox &bbox, const Rgba &color);
	static bool setupScreenQuad(MeshPrim*& mesh, const Rect &rect, const Rgba &color, Material *material=nullptr, const Vector4 &st = Vector4(0,0,1,1));
	static bool setupHexahedron(MeshPrim*& mesh, Vector3 vertes[8]);

private:
	int m_numVertexes;
	MeshVertex *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;
	bool m_isStriped;
};

//--------------------------------------------------------------------------
// class TextPrim
//--------------------------------------------------------------------------
class Font;
class AX_API TextPrim : public Primitive
{
public:
	enum Format {
		// style flags
		Bold = 1,
		Underline = 2,
		Italic = 4,
		DropShadow = 8,
		Blink = 0x10,
		ScaleByHorizon = 0x20,		// stretch to full target rect, so don't need align
		ScaleByVertical = 0x40,
	};

	// horizon align
	enum HorizonAlign {
		Center, Left, Right
	};

	// vertical align
	enum VerticalAlign {
		VCenter, Top, Bottom
	};

	TextPrim(Hint hint);
	virtual ~TextPrim();

	void init(const Rect &rect, Rgba color, float aspect, int format, Font *font, const std::string &text);
	void initSimple(const Vector3 &xyz, Rgba color, const std::string &text, bool fixedWidth = true);
	void clear();

	const Rect &getRect() const { return m_rect; }
	const Vector3 &getPosition() const { return m_position; }
	const bool getIsSimpleText() const { return m_isSimpleText; }
	const Rgba getColor() const { return m_color; }
	const float getAspect() const { return m_aspect; }
	const int getFormat() const { return m_format; }
	const std::string &getText() const { return m_text; }
	Font *getFont() const { return m_font; }
	HorizonAlign getHorizonAlign() const { return m_horizonAlign; }
	VerticalAlign getVerticalAlign() const { return m_verticalAlign; }
	void setHorizonAlign(HorizonAlign align) { m_horizonAlign = align; }
	void setVerticalAlign(VerticalAlign align) { m_verticalAlign = align; }

	virtual void draw(Technique tech) {}
	virtual void sync() {}

	// static helper function
	static TextPrim *createSimpleText(Hint hint, const Vector3 &xyz, const Rgba &color, const std::string &text, bool fixedWidth = true);
	static TextPrim *createText(Hint hint, const Rect &rect, Font *font, const std::string &text, const Rgba &color=Rgba::White, HorizonAlign halign=Center, VerticalAlign valign=VCenter, int format=0, float aspect = 1.0f);

private:
	Rect m_rect;				// draw on this rect
	Vector3 m_position;			// for simple text
	bool m_isSimpleText;
	Rgba m_color;				// text color
	float m_aspect;
	int m_format;				// format flags
	Font *m_font;				// font used
	std::string m_text;				// string to draw
	HorizonAlign m_horizonAlign;
	VerticalAlign m_verticalAlign;
};

//--------------------------------------------------------------------------
// class ChunkPrim
//--------------------------------------------------------------------------

class AX_API ChunkPrim : public Primitive
{
public:
	enum {
		MAX_LAYERS = 4
	};
	struct Layer {
		Texture *alphaTex;
		Material *detailMat;
		Vector2 scale;
		bool isVerticalProjection;
	};

	ChunkPrim(Hint hint);
	virtual ~ChunkPrim();

	void init(int numverts, int numindexes);
	void clear();

	int getNumVertexes() const;
	const ChunkVertex *getVertexesPointer() const;
	ChunkVertex *lockVertexes();
	void unlockVertexes();

	int getNumIndexes() const;
	const ushort_t *getIndexesPointer() const;
	ushort_t *lockIndexes();
	void unlockIndexes();

	// terrain properties
	void setTerrainRect(const Vector4 &rect);
	Vector4 getTerrainRect() const;

	// zone properties
	void setZoneRect(const Vector4 &rect);
	Vector4 getZoneRect() const;
	void setColorTexture(Texture *color_texture);
	Texture *getColorTexture();
	void setNormalTexture(Texture *normal);
	Texture *getNormalTexture();

	// chunk properties
	void setChunkRect(const Vector4 &rect);
	Vector4 getChunkRect() const;
	void setNumLayers(int n);
	int getNumLayers() const;
	void setLayers(int index, Texture *alpha, Material *detail, const Vector2 &scale, bool isVerticalProjection = false);
	Texture *getLayerAlpha(int index) const;
	Material *getLayerDetail(int index) const;
	Vector2 getLayerScale(int index) const;
	void setLayerVisible(bool visible) { m_layerVisible = visible; m_isDirty = true; }
	bool getLayerVisible() const { return m_layerVisible; }
	bool isLayerVerticalProjection(int index) const;

	bool isZonePrim() const { return m_isZonePrim; }
	void setIsZonePrim(bool val) { m_isZonePrim = val; }

	virtual void draw(Technique tech);
	virtual void sync();

private:
	int m_numVertexes;
	ChunkVertex *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;

	Vector4 m_terrainRect;

	Vector4 m_zoneRect;
	Texture *m_colorTexture;
	Texture *m_normalTexture;

	Vector4 m_chunkRect;
	int m_numLayers;
	Layer m_layers[MAX_LAYERS];
	bool m_layerVisible;
	bool m_isZonePrim;
};

inline void ChunkPrim::setZoneRect(const Vector4 &rect)
{
	m_zoneRect = rect;
}
inline Vector4 ChunkPrim::getZoneRect() const
{
	return m_zoneRect;
}

//--------------------------------------------------------------------------
// class GroupPrim
//
// this prim will not free grouped primitives
//--------------------------------------------------------------------------

class AX_API GroupPrim : public Primitive
{
public:
	GroupPrim(Hint hint);
	virtual ~GroupPrim();

	void addPrimitive(Primitive *prim, bool needfree = true);
	int getPrimitiveCount() const;
	Primitive *getPrimitive(int index);
	void clear();

	virtual void draw(Technique tech);
	virtual void sync();

private:
	typedef std::vector<bool> BoolSeq;
	Primitives m_primitives;
	BoolSeq m_needFrees;
};

//--------------------------------------------------------------------------
// class RefPrim
//
// this prim will not free refered primitive
//--------------------------------------------------------------------------

class AX_API RefPrim : public Primitive
{
public:
	RefPrim(Hint hint);
	virtual ~RefPrim();

	Primitive *getRefered() const;
	void setRefered(Primitive *refered);

	void init(Primitive *refered, int numindexes = 0);

	int getNumIndexes() const;
	const ushort_t *getIndexesPointer() const;
	ushort_t *lockIndexes();
	void unlockIndexes();

	virtual void draw(Technique tech);
	virtual void sync();

private:
	Primitive *m_refered;

	// override refered primitive's indexes
	int m_numIndexes;
	ushort_t *m_indexes;
};

//--------------------------------------------------------------------------
// class GeoInstancing
//
// geometry instance primitive. this primitive will not free instanced primitive
//--------------------------------------------------------------------------

class AX_API InstancePrim : public Primitive
{
public:
	// per instance parameter
	struct Param {
		Matrix worldMatrix;
		Vector4 userDefined;		// user defined param
	};

	InstancePrim(Hint hint);
	virtual ~InstancePrim();

	Primitive *getInstanced() const;
	void init(Primitive *instanced, int numInstances);

	void setInstance(int index, const Param &param);
	void setInstance(int index, const Matrix &mtx, const Vector4 &user);
	int getNumInstance() const;
	const Param &getInstance(int index) const;
	const Param *getAllInstances() const;

	virtual void draw(Technique tech);
	virtual void sync();

private:
	int m_numInstances;
	Primitive *m_instanced;
	Param* m_params;
	InstanceObject *m_instanceObject;
};

#if 0
//--------------------------------------------------------------------------
// class PrimitiveManager
//--------------------------------------------------------------------------

class AX_API PrimitiveManager
{
public:
	PrimitiveManager();
	virtual ~PrimitiveManager();

	void hintUncache(Primitive *prim);

protected:
	bool isStatic(int id) { return(size_t(id) & FRAME_FLAG) == 0; }
	bool isFrameHandle(int id) { return !isStatic(id); }

protected:
	enum {
		FRAME_FLAG = 0x80000000,
		INDEX_MASK = ~FRAME_FLAG,
	};
	std::list<int> m_waitUncache;
};
#endif

AX_END_NAMESPACE

#endif // AX_RENDER_PRIMITIVE_H
