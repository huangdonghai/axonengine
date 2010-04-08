#ifndef AX_PRIMITIVEP_H
#define AX_PRIMITIVEP_H

AX_BEGIN_NAMESPACE

class PrimitiveBackend : public RenderBackend
{
	AX_DECLARE_DATA(PrimitiveData);
public:

	PrimitiveBackend();
	virtual ~PrimitiveBackend();

private:
	bool m_isDirty;		// dirty
	bool m_isVertexBufferDirty;
	bool m_isIndexBufferDirty;
	bool m_isWorldSpace; // primitive already in world space, so don't need model transform

	MaterialBackend *m_material;
	TextureBackend *m_lightMap;

	bool m_isMatrixSet;
	Matrix4 m_matrix;

	int m_activedIndexes;

	int m_chainId;
	Interaction *m_headInteraction;
};

class GeometryPB : public PrimitiveBackend
{
public:
private:
	// vertex info
	VertexType m_vertexType;
	VertexBuffer m_vertexObject;

	// index info
	IndexBuffer m_indexObject;
	ElementType m_elementType;

	float m_geometrySize;		// point size or line width
};

class TextPB : public PrimitiveBackend
{

};

class ChunkPB : public PrimitiveBackend {};

class GroupPB : public PrimitiveBackend {};

class RefPB : public PrimitiveBackend {};

class InstancePB : public PrimitiveBackend {};

class PrimitiveData : public RenderData
{
	AX_DECLARE_BACKEND(PrimitiveBackend);

public:
	PrimitiveData();
	virtual ~PrimitiveData();

	Type getType() const;

	void setWorldSpace(bool val);

	void setMatrix(const Matrix4 &matrix);
	void disableMatrix();

	void setMaterial(const Material2 &material);
	void setLightMap(const Texture2 &lm);

	void setActivedIndexes(int val);

	virtual void reallyDelete()
	{
		if (m_hint == Primitive2::HintFrame) {
			this->~PrimitiveData();
			return;
		}

		delete this;
	}

	template <class T>
	static T *createPrimitive_(Primitive2::Hint hint)
	{
		if (hint == Primitive2::HintFrame) {
			T *result = g_renderQueue->allocType<T>(1);
			new (result) T(hint);
			return result;
		}

		return new T(hint);
	}

	template <class T>
	static T *allocType(Primitive2::Hint hint, int num = 1)
	{
		if (hint == Primitive2::HintFrame) {
			return g_renderQueue->allocType<T>(num);
		}

		return Malloc(sizeof(T) * num);
	}

	static void freeType(Primitive2::Hint hint, void *p)
	{
		if (hint == Primitive2::HintFrame)
			return;

		Free(p);
	}

protected:
	const Primitive2::Hint m_hint;
	bool m_isDirty;		// dirty
	bool m_isVertexBufferDirty;
	bool m_isIndexBufferDirty;
	bool m_isWorldSpace; // primitive already in world space, so don't need model transform

	Primitive2::Type m_type;
	Material2 m_material;
	Texture2 m_lightMap;

	bool m_isMatrixSet;
	Matrix4 m_matrix;

	int m_activedIndexes;
};

class LineData : public PrimitiveData
{
public:
	LineData(Primitive2::Hint);

	DebugVertex *lockVertexBuffer();

private:
	int m_numVertexes;
	DebugVertex *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;
	float m_lineWidth;
};

class MeshData : public PrimitiveData
{
public:
private:
	int m_numVertexes;
	MeshVertex *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;
	bool m_isStriped;
};

class TextData : public PrimitiveData
{
public:
private:
	Rect m_rect;				// draw on this rect
	Vector3 m_position;			// for simple text
	bool m_isSimpleText;
	Rgba m_color;				// text color
	float m_aspect;
	int m_format;				// format flags
	Font *m_font;				// font used
	String m_text;				// string to draw
//	HorizonAlign m_horizonAlign;
//	VerticalAlign m_verticalAlign;
};

class ChunkData : public PrimitiveData
{
public:
	enum {
		MAX_LAYERS = 4
	};

	struct Layer {
		Texture2 alphaTex;
		Material2 detailMat;
		Vector2 scale;
		bool isVerticalProjection;
	};

private:
	int m_numVertexes;
	ChunkVertex *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;

	Vector4 m_terrainRect;

	Vector4 m_zoneRect;
	TexturePtr m_colorTexture;
	TexturePtr m_normalTexture;

	Vector4 m_chunkRect;
	int m_numLayers;
	Layer m_layers[MAX_LAYERS];
	bool m_layerVisible;
	bool m_isZonePrim;
};

class GroupData : public PrimitiveData
{
public:
private:
	Sequence<Primitive2*> m_primitives;
};

class RefData : public PrimitiveData
{
public:
private:
	Primitive2 *m_refered;

	// override refered primitive's indexes
	int m_numIndexes;
	ushort_t *m_indexes;
};

class InstanceData : public PrimitiveData
{
public:
	// per instance parameter
	struct Param {
		Matrix worldMatrix;
		Vector4 userDefined;		// user defined param
	};

private:
	Primitive2 *m_instanced;

	int numInstance;
	Param* m_params;
};

AX_END_NAMESPACE

#endif // AX_PRIMITIVEP_H
