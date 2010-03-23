#ifndef AX_PRIMITIVEP_H
#define AX_PRIMITIVEP_H

AX_BEGIN_NAMESPACE

class PrimitiveBackend : public RenderResource
{
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
public:
	PrimitiveData();
	virtual ~PrimitiveData();

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
	static T *allocType<T>(Primitive2::Hint hint, int num = 1)
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
	PrimitiveBackend *m_backend;

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

class LinePF : public PrimitiveData
{
public:

private:
	int m_numVertexes;
	DebugVertex *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;
	float m_lineWidth;
};

class MeshPF : public PrimitiveData
{};

AX_END_NAMESPACE

#endif // AX_PRIMITIVEP_H
