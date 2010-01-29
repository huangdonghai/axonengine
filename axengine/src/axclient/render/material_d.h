#ifndef AX_MATERIALDATA_H
#define AX_MATERIALDATA_H

AX_BEGIN_NAMESPACE

class MaterialData
{
public:
private:
	int m_ref;

	FixedString m_key;
	MaterialDecl *m_decl;

	bool m_isDirty;

	Color3 m_diffuse;
	float m_opicity;
	Color3 m_specular;
	float m_shiness;

	const ShaderInfo *m_shaderInfo;
	FixedString m_shaderName;

	// texgen
	bool m_texgen;
	Matrix4 m_texgenMatrix;
};

AX_END_NAMESPACE

#endif // AX_MATERIALDATA_H
