#ifndef AX_MATERIALDATA_H
#define AX_MATERIALDATA_H

AX_BEGIN_NAMESPACE

class MaterialData
{
public:
private:
	FixedString m_key;
	MaterialDecl *m_decl;

	bool m_isDirty;

	Color3 m_diffuse;
	Color3 m_specular;
	float m_shiness;
	float m_detailScale;
	bool m_haveDetail;

	const ShaderInfo *m_shaderInfo;
	FixedString m_shaderName;
	TexturePtr m_textures[SamplerType::NUMBER_ALL];
	TexGen m_texgens[SamplerType::NUMBER_ALL];
	ShaderParams m_shaderParams;

	// texgen etc...
	bool m_baseTcAnim;
	Matrix4 m_baseTcMatrix;
	RgbGen m_rgbGen;
	AlphaGen m_alphaGen;
	VertexDeform m_deform;

	bool m_features[Shader::MAX_FEATURES];
	int m_literals[Shader::MAX_LITERALS];

	// pixel to texel conversion
	bool m_p2tEnabled;
	int m_p2tWidth, m_p2tHeight;
};

AX_END_NAMESPACE

#endif // AX_MATERIALDATA_H
