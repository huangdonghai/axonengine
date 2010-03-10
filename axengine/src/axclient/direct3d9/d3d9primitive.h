/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_D3D9PRIMITIVE_H
#define AX_D3D9PRIMITIVE_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class D3D9Primitive
//--------------------------------------------------------------------------

class D3D9Primitive
{
public:
	enum Type {
		kAbstract,
		kGeometry,
		kText,
		kTerrain,
		kGroup,
		kRef,
		kInstance,
		
		kNumber
	};
	D3D9Primitive();
	virtual ~D3D9Primitive();

	virtual void initialize(Primitive *source_primitive) = 0;
	virtual void finalize() = 0;
	virtual void update() = 0;
	virtual void draw(Technique tech) = 0;
	virtual Type getType() = 0 { return kAbstract; }

	void setOverloadMaterial(Material *mat) {
		m_overloadMaterial = mat;
	}
	void unsetOverloadMaterial() {
		m_overloadMaterial = NULL;
	}

	Material *getMaterial() { return m_material; }
	inline bool isMatrixSet() const { return m_isMatrixSet; }
	const Matrix4 &getMatrix() const { return m_matrix; }

protected:
	Primitive *m_src;
	Primitive::Hint m_hint;
	Material *m_material;

public:
	Material *m_overloadMaterial;
	D3D9InstancedBuffer *m_instanceBuffer;
	D3D9IndexObject *m_overloadedIndexObj;
	int m_activeIndexes;

	bool m_isMatrixSet;
	Matrix4 m_matrix;

	Texture *m_lightmap;
};

//--------------------------------------------------------------------------
// class D3D9geometry
//--------------------------------------------------------------------------

class D3D9geometry : public D3D9Primitive
{
public:
	typedef D3D9VertexObject::VertexType VertexType;

	D3D9geometry();
	virtual ~D3D9geometry();

	virtual void initialize(Primitive *source_primitive);
	virtual void finalize();
	virtual void update();
	virtual void draw(Technique tech);
	virtual Type getType() { return kGeometry; }

	void bindVertexBuffer();
	void unbindVertexBuffer();
	void drawElements();

protected:
	void initPoint();
	void initLine();
	void initMesh();

	void updatePoint();
	void updateLine();
	void updateMesh();

protected:
	// vertex info
	VertexType m_vertexType;
	D3D9VertexObject m_vertexObject;

	// index info
	D3D9IndexObject m_indexObject;
	D3DPRIMITIVETYPE m_d3dPrimitiveType;

	float m_geometrySize;		// point size or line width
};

//--------------------------------------------------------------------------
// class D3D9text
//--------------------------------------------------------------------------

class D3D9text : public D3D9Primitive
{
public:
	D3D9text();
	virtual ~D3D9text();

	virtual void initialize(Primitive *source_primitive);
	virtual void finalize();
	virtual void update();
	virtual void draw(Technique tech);
	virtual Type getType() { return kText; }

private:
	Rect m_rect;				// draw on this rect
	Vector3 m_position;			// for simple text
	bool m_isSimpleText;
	float m_aspect;
	int m_format;			// format flags
	Font *m_font;				// font used
	String m_text;				// string to draw
	TextPrim::HorizonAlign m_horizonAlign;
	TextPrim::VerticalAlign m_verticalAlign;
	Rgba m_color;
};

//--------------------------------------------------------------------------
// class D3D9terrain
//--------------------------------------------------------------------------

class D3D9terrain : public D3D9geometry
{
public:
	D3D9terrain();
	virtual ~D3D9terrain();

	// implement D3D9Primitive
	virtual void initialize(Primitive *source_primitive);
	virtual void finalize();
	virtual void update();
	virtual void draw(Technique tech);
	virtual Type getType() { return kTerrain; }

protected:

private:
	Vector4 m_terrainRect;

	Texture *m_colorTexture;
	Texture *m_normalTexture;
	Vector4 m_zoneRect;

	Vector4 m_chunkRect;
	int m_numLayers;
	ChunkPrim::Layer m_layers[ChunkPrim::MAX_LAYERS];
	bool m_layerVisible;
	bool m_isZonePrim;
	Primitive::Hint m_indexHint;
};

//--------------------------------------------------------------------------
// class D3D9group
//--------------------------------------------------------------------------

class D3D9group : public D3D9Primitive
{
public:
	D3D9group();
	virtual ~D3D9group();

	virtual void initialize(Primitive *source_primitive);
	virtual void finalize();
	virtual void update();
	virtual void draw(Technique tech);
	virtual Type getType() { return kGroup; }

private:
	Sequence<int>		m_primitives;
};

//--------------------------------------------------------------------------
// class D3D9ref
//--------------------------------------------------------------------------

class D3D9ref : public D3D9Primitive
{
public:
	D3D9ref();
	virtual ~D3D9ref();

	virtual void initialize(Primitive *source_primitive);
	virtual void finalize();
	virtual void update();
	virtual void draw(Technique tech);
	virtual Type getType() { return kRef; }

private:
	int m_refed;
	D3D9IndexObject m_indexObject;
};

//--------------------------------------------------------------------------
// class D3D9instance
//--------------------------------------------------------------------------

class D3D9instance : public D3D9Primitive
{
public:
	D3D9instance();
	virtual ~D3D9instance();

	virtual void initialize(Primitive *src);
	virtual void finalize();
	virtual void update();
	virtual void draw(Technique tech);
	virtual Type getType() { return kInstance; }

private:
	int m_instanced;
	D3D9InstancedBuffer m_buffer;
};

class D3D9primitivemanager : public PrimitiveManager
{
public:
	D3D9primitivemanager();
	~D3D9primitivemanager();

	void syncFrame();

	int cachePrimitive(Primitive *prim);
	void uncachePrimitive(Primitive *prim);
	D3D9Primitive *getPrimitive(int handle);

	void onDeviceLost();
	void onReset();

protected:
	void findStaticFreeSlot(int &handle);
	void linkId(int id, D3D9Primitive *glprim);
	D3D9Primitive *createPrim(Primitive *prim);
	void freePrim(D3D9Primitive *prim);

private:
	enum {
		MAX_PRIMITIVES = 8 * 1024,
	};

	int m_frameId;

	int m_numStaticPrims;
	D3D9Primitive *m_staticPrims[MAX_PRIMITIVES];

	int m_numFramePrims;
	D3D9Primitive *m_framePrims[MAX_PRIMITIVES];

	void *m_freePrimLink;

	int m_numWaitForDelete;
	int m_waitForDelete[MAX_PRIMITIVES];

	BlockAlloc<D3D9geometry>	m_geometryAlloc;
	BlockAlloc<D3D9text>		m_textAlloc;
	BlockAlloc<D3D9terrain>		m_terrainAlloc;
	BlockAlloc<D3D9group>		m_groupAlloc;
	BlockAlloc<D3D9ref>			m_refAlloc;
	BlockAlloc<D3D9instance>	m_instanceAlloc;
};

inline D3D9Primitive *D3D9primitivemanager::getPrimitive(int handle)
{
	if (handle == -1) {
		return nullptr;
	}

	size_t h = (size_t)handle;
	if (h & FRAME_FLAG)
		return m_framePrims[(h & INDEX_MASK)-1];
	else
		return m_staticPrims[(h & INDEX_MASK)-1];
}

AX_END_NAMESPACE


#endif // end guardian

