#ifndef AX_MATERIALDATA_H
#define AX_MATERIALDATA_H

AX_BEGIN_NAMESPACE

class MaterialData;
class MaterialBackend : public RenderBackend
{
public:

	void sync(MaterialData *src);

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

class MaterialData : public RenderData
{
public:
	MaterialData(const String &name);
	~MaterialData();

	bool isWireframe() const;
	bool isPhysicsHelper() const;

	void setDiffuse(const Color3 &v);
	void setSpecular(const Color3 &v);
	void setShiness(float shiness);
	void setDetailScale(float scale);
	float getDetailScale() const;
	bool haveDetail() const;

	Color3 getMatDiffuse() const;
	Color3 getMatSpecular() const;
	float getMatShiness() const;

	// features and shader parameter
	void setFeature(int index, bool enabled);
	bool isFeatureEnabled(int index) const;
	void setLiteral(int index, int value);
	int getLiteral(int index) const;
	void clearFeatures();
	void clearLiterals();

	// texture setting and getting
	const Texture2 &getTexture(int sample) const;
	void setTexture(int sampler, const Texture2 &tex);

	// parameter setting and getting
	void setParameter(const String &name, int num, const float *ptr);
	const ShaderParams &getParameters() const;

	void setBaseTcMatrix(const Matrix4 &matrix);
	bool isBaseTcAnim() const;
	const Matrix4 &getBaseTcMatrix() const;

	// pixel to texel
	void setPixelToTexel(int width, int height);
	bool isPixelToTexelEnabled() const;
	int getPixelToTexelWidth() const;
	int getPixelToTexelHeight() const;

private:
	const MaterialDecl *m_decl;

	Color3 m_diffuse;
	float m_opicity;
	Color3 m_specular;
	float m_shiness;
	float m_detailScale;
	bool m_haveDetail;

	FixedString m_shaderName;
	Texture2 m_textures[SamplerType::NUMBER_ALL];
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
