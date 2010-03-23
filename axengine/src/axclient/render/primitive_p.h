#ifndef AX_PRIMITIVEP_H
#define AX_PRIMITIVEP_H

AX_BEGIN_NAMESPACE

class PrimitiveBackend : public RenderResource
{
public:
private:
	bool m_isDirty;		// dirty
	bool m_isVertexBufferDirty;
	bool m_isIndexBufferDirty;
	bool m_isWorldSpace; // primitive already in world space, so don't need model transform

	int m_cachedId;		// used by render driver
	int m_cachedFrame;

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
};

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
	typedef DebugVertex VertexType;

private:
	int m_numVertexes;
	VertexType *m_vertexes;
	int m_numIndexes;
	ushort_t *m_indexes;
	float m_lineWidth;
};

class MeshPF : public PrimitiveData
{};

AX_END_NAMESPACE

#endif // AX_PRIMITIVEP_H
