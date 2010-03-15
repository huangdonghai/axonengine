#ifndef AX_PRIMITIVEP_H
#define AX_PRIMITIVEP_H

AX_BEGIN_NAMESPACE

class PrimitiveBackend : public RenderFrontendResource
{
public:
private:
	bool m_isDirty;		// dirty
	bool m_isVertexBufferDirty;
	bool m_isIndexBufferDirty;
	bool m_isWorldSpace; // primitive already in world space, so don't need model transform

	int m_cachedId;		// used by render driver
	int m_cachedFrame;

	Element::ElementType m_type;
	MaterialBackend *m_material;
	TextureBackend *m_lightMap;

	bool m_isMatrixSet;
	Matrix4 m_matrix;

	int m_activedIndexes;

	int m_chainId;
	Interaction *m_headInteraction;
};

AX_END_NAMESPACE

#endif // AX_PRIMITIVEP_H
