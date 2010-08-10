/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_MATERIAL_H
#define AX_RENDER_MATERIAL_H

AX_BEGIN_NAMESPACE

class AX_API Material
{
public:
	Material(const String &name);
	~Material();

	// implement RefObject
	virtual void deleteThis();

	// must be success
	bool init(const FixedString &name);

	const ShaderInfo *getShaderInfo() const { return m_shaderInfo; }
	const FixedString &getShaderName() const { return m_shaderName; }
	bool isWireframe() const;
	bool isPhysicsHelper() const;

	RenderStateId getRenderStateId() const { return m_renderStateId; }
	void setRenderStateId(RenderStateId id) { m_renderStateId = id; }

	MaterialDecl::SortHint getSortHint() const { return m_sortHint; }

	void setTextureSet(const String &texname);

	void setDiffuse(const Vector3 &v);
	void setSpecular(const Vector3 &v);
	void setShiness(float shiness) { m_shiness = shiness; }
	void setDetailScale(float scale) { m_detailScale = scale; }
	float getDetailScale() const { return m_detailScale; }
	bool haveDetail() const { return m_haveDetail; }

	Vector3 getMatDiffuse() const;
	Vector3 getMatSpecular() const;
	float getMatShiness() const;

	// features and shader parameter
	void setFeature(int index, bool enabled);
	bool isFeatureEnabled(int index) const;
	void setLiteral(int index, int value);
	int getLiteral(int index) const;
	void clearFeatures();
	void clearLiterals();

	// texture setting and getting
	Texture *getTexture(int sample) const;
	void setTexture(int sampler, Texture *tex);

	// parameter setting and getting
	void setParameter(const String &name, int num, const float *ptr);
	const ShaderParams &getParameters() const;

	// shader macro
	const ShaderMacro &getShaderMacro();

	void setBaseTcMatrix(const Matrix4 &matrix);
	bool isBaseTcAnim() const { return m_baseTcAnim; }
	const Matrix4 *getBaseTcMatrix() const { return &m_baseTcMatrix; }

#if 0
	// pixel to texel
	void setPixelToTexel(int width, int height);
	bool isPixelToTexelEnabled() const { return m_p2tEnabled; }
	int getPixelToTexelWidth() const { return m_p2tWidth; }
	int getPixelToTexelHeight() const { return m_p2tHeight; }

	// management
	static Material *load(const String &name);
	static Material *loadUnique(const String &name);
	static void initManager();
	static void finalizeManager();
#endif
	static FixedString normalizeKey(const String &name);
#if 0
	static void syncFrame();
#endif
	static void matlist_f(const CmdArgs &args);
	// end management

private:
	FixedString m_key;
	MaterialDecl *m_decl;
	RenderStateId m_renderStateId;
	MaterialDecl::SortHint m_sortHint;

	bool m_shaderMacroNeedRegen;
	ShaderMacro m_shaderMacro;

	Vector3 m_diffuse;
	Vector3 m_specular;
	float m_shiness;
	float m_detailScale;
	bool m_haveDetail;

	const ShaderInfo *m_shaderInfo;
	FixedString m_shaderName;
	Texture *m_textures[MaterialTextureId::MaxType];
	ShaderParams m_shaderParams;
	ConstBuffer *m_localUniforms;

	// texgen etc...
	bool m_baseTcAnim;
	Matrix4 m_baseTcMatrix;

	bool m_features[MaterialDecl::MAX_FEATURES];
	int m_literals[MaterialDecl::MAX_LITERALS];

#if 0
	// pixel to texel conversion
	bool m_p2tEnabled;
	int m_p2tWidth, m_p2tHeight;
#endif

private:
	// management
	typedef Dict<FixedString, Material*> MaterialDict;
	static MaterialDict ms_materialDict;
//	static IntrusiveList<Material, &Material::m_needDeleteLink> ms_needDeleteLinkHead;

	static void _deleteMaterial(Material *mat);

public:
	// delete link
	IntrusiveLink<Material> m_needDeleteLink;
};

inline void Material::setDiffuse(const Vector3 &v)
{
	m_diffuse = v;
}

inline void Material::setSpecular(const Vector3 &v)
{
	m_specular = v;
}


inline Vector3 Material::getMatDiffuse() const
{
	return m_diffuse;
}

inline Vector3 Material::getMatSpecular() const
{
	return m_specular;
}

inline float Material::getMatShiness() const
{
	return m_shiness;
}

inline void Material::setFeature(int index, bool enabled)
{
	AX_ASSERT(index >= 0 && index < MaterialDecl::MAX_FEATURES);
	if (m_features[index] != enabled) {
		m_shaderMacroNeedRegen = true;
		m_features[index] = enabled;
	}
}

inline bool Material::isFeatureEnabled(int index) const
{
	AX_ASSERT(index >= 0 && index < MaterialDecl::MAX_FEATURES);
	return m_features[index];
}

inline void Material::setLiteral(int index, int value)
{
	AX_ASSERT(index >= 0 && index < MaterialDecl::MAX_LITERALS);
	if (m_literals[index] != value) {
		m_literals[index] = value;
		m_shaderMacroNeedRegen = true;
	}
}
inline int Material::getLiteral(int index) const
{
	AX_ASSERT(index >= 0 && index < MaterialDecl::MAX_LITERALS);
	return m_literals[index];
}

inline void Material::clearFeatures()
{
	m_shaderMacroNeedRegen = true;
	TypeZeroArray(m_features);
}

inline void Material::clearLiterals()
{
	m_shaderMacroNeedRegen = true;
	TypeZeroArray(m_literals);
}

#if 0
inline void Material::setPixelToTexel(int width, int height)
{
	m_p2tEnabled = true;
	m_p2tWidth = width;
	m_p2tHeight = height;
}
#endif

inline Texture *Material::getTexture(int sampler) const
{
	AX_ASSERT(sampler >= 0 && sampler < MaterialTextureId::MaxType);
	return m_textures[sampler];
}

inline void Material::setTexture(int sampler, Texture *tex)
{
	m_shaderMacroNeedRegen = true;
	AX_ASSERT(sampler >= 0 && sampler < MaterialTextureId::MaxType);
	m_textures[sampler] = tex;
}


AX_END_NAMESPACE

#endif // AX_MATERIAL_H
