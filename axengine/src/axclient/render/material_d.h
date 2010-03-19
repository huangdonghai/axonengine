#ifndef AX_MATERIALDATA_H
#define AX_MATERIALDATA_H

AX_BEGIN_NAMESPACE

class MaterialBackend : public RenderBackendResource
{
public:

	void sync(NewMaterial *src);

private:
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

class MaterialFrontend : public RenderFrontendResource
{
public:
private:
	const MaterialDecl *m_decl;

	Color3 m_diffuse;
	float m_opicity;
	Color3 m_specular;
	float m_shiness;
	float m_detailScale;
	bool m_haveDetail;

	FixedString m_shaderName;
	NewTexture m_textures[SamplerType::NUMBER_ALL];
	ShaderParams m_shaderParams;

	// texgen etc...
	bool m_baseTcAnim;
	Matrix4 m_baseTcMatrix;

	bool m_features[Shader::MAX_FEATURES];
	int m_literals[Shader::MAX_LITERALS];

	// pixel to texel conversion
	bool m_p2tEnabled;
	int m_p2tWidth, m_p2tHeight;
};

AX_END_NAMESPACE

#endif // AX_MATERIALDATA_H
