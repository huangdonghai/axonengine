#ifndef AX_MATERIALDATA_H
#define AX_MATERIALDATA_H

AX_BEGIN_NAMESPACE

class MaterialDecl : public KeyedObject
{
	friend class Material;

public:
	class TextureDef {
	public:
		std::string file;
		SamplerDesc desc;
	};

	bool tryLoad(const std::string &name);

	const std::string &getShaderName() { return m_shaderName; }
	TextureDef *getTextureDef(MaterialTextureId maptype) { return m_textures[maptype]; }
	bool isWireframed() const { return m_wireframed; }
	Material::Flags getFlags() const { return m_flags; }
	void setFlags(Material::Flags flags) { m_flags = flags; }
	Color3 getDiffuse() const { return m_diffuse; }
	const bool *getFeatures() const { return m_features; }

	friend class MaterialDeclManager;
	bool isDefaulted() const { return this == ms_defaulted; }

	// management
	static MaterialDecl *load(const std::string &name);
	static void initManager();
	static void finalizeManager();

protected:
	MaterialDecl();
	virtual ~MaterialDecl();

	static FixedString normalizeKey(const std::string &name);

private:
	Material::SortHint m_sortHint;
	std::string m_shaderName;
	int m_shaderGenMask;
	Material::Flags m_flags;
	SurfaceType m_surfaceType;
	TextureDef *m_textures[MaterialTextureId::MaxType];
	FastParams *m_fastParams;
	Color3 m_diffuse, m_specular, m_emission;
	float m_shiness;
	float m_opacity;
	Vector4 m_detailScale;

	bool m_depthWrite : 1;
	bool m_depthTest : 1;
	bool m_wireframed : 1;
	Material::BlendMode m_blendMode : 4;
	RasterizerDesc::CullMode m_cullMode : 4;

	// features and literals
	bool m_features[Material::MAX_FEATURES];

	// manager
	static MaterialDecl *ms_defaulted;
	typedef Dict<FixedString, MaterialDecl*> MaterialDeclDict;
	typedef DictSet<FixedString> FailureSet;
	static MaterialDeclDict ms_declDict;
	static FailureSet ms_failureSet;
};

AX_END_NAMESPACE

#endif // AX_MATERIALDATA_H
