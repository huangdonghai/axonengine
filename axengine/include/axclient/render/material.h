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

#define AX_DECL_SURFACETYPE \
	AX_ENUM(Dust) \
	AX_ENUM(Metal) \
	AX_ENUM(Sand) \
	AX_ENUM(Wood) \
	AX_ENUM(Grass) \
	AX_ENUM(Snow) \
	AX_ENUM(Glass) \
	AX_ENUM(Water) \
	AX_ENUM(MaxType)

struct SurfaceType {
	enum Type {
#define AX_ENUM(x) x,
		AX_DECL_SURFACETYPE
#undef AX_ENUM
		// Dust, Metal, Sand, Wood, Grass, Snow, Glass, Water
	};
	AX_DECLARE_ENUM(SurfaceType);

	const char *toString() {
		switch (t) {
#define AX_ENUM(x) case x: return #x;
			AX_DECL_SURFACETYPE
#undef AX_ENUM
		}

		return "INVALID";
	}

	static int fromString(const char *str) {
#define AX_ENUM(x) if (Strequ(str, #x)) { return x; }
		AX_DECL_SURFACETYPE
#undef AX_ENUM
			return 0;
	}
};

struct GlobalTextureId {
	enum Type {
		RtDepth, Rt0, Rt1, Rt2, Rt3,
		SceneColor, ShadowMap,

		MaxType
	};
	AX_DECLARE_ENUM(GlobalTextureId);
};

struct MaterialTextureId {
	enum Type {
		// material sampler
		Diffuse, Normal, Specular, Detail, DetailNormal, Opacit, Emission,
		Displacement, Env,

		// terrain sampler
		TerrainColor, TerrainNormal, LayerAlpha,

		// other
		Reflection, LightMap,

		MaxType
	};
	AX_DECLARE_ENUM(MaterialTextureId);
};

class FastParams
{
public:
	FastParams() { m_numItems = 0; }
	~FastParams() {}

	void clear() { m_numItems = 0; }
	void addParam(const FixedString &name, int num, const float *data)
	{
		AX_ASSURE(m_numItems < NUM_ITEMS);
		Item &item = m_items[m_numItems];
		item.nameId = name.id();
		item.count = num;
		if (m_numItems == 0) {
			item.offset = 0;
		} else {
			Item &preItem = m_items[m_numItems - 1];
			item.offset = preItem.offset + preItem.count;
		}
		::memcpy(m_floatData+item.offset, data, num * sizeof(float));
		m_numItems++;
	}

public:
	enum { NUM_ITEMS = 16, NUM_FLOATDATA = 256 };
	struct Item {
		int nameId;
		int offset;
		int count;
	};
	int m_numItems;
	Item m_items[NUM_ITEMS];
	float m_floatData[NUM_FLOATDATA];
};

class MaterialDecl;

class AX_API Material
{
	friend class RenderContext;
	friend class ChunkPrim;

public:
	enum SortHint {
		SortHint_Opacit,
		SortHint_Decal,
		SortHint_UnderWater,
		SortHint_Water,
		SortHint_AboveWater
	};

	enum {
		MAX_FEATURES = 8,
	};

	// defined in material
	enum WaveType {
		WaveNone,
		WaveSin,
		WaveSquare,
		WaveTriangle,
		WaveSawtooth,
		WaveInversesawtooth,
		WaveNoise
	};

	enum Flag {
		Flag_NoDraw = 1,
		Flag_PhysicsHelper = 0x80000,
	};

	enum BlendMode {
		BlendMode_Disabled,
		BlendMode_Add,
		BlendMode_Blend,
		BlendMode_Modulate
	};

	typedef Flags_<Flag> Flags;
	Material(const std::string &name);
	~Material();

	// must be success
	bool init(const FixedString &name);

	const ShaderInfo *getShaderInfo() const { return m_shaderInfo; }
	const FixedString &getShaderName() const { return m_shaderName; }
	bool isWireframe() const;
	bool isPhysicsHelper() const;

	SortHint getSortHint() const { return m_sortHint; }

	void setTextureSet(const std::string &texname);

	void setDiffuse(const Color3 &v);
	void setSpecular(const Color3 &v);
	void setShiness(float shiness) { m_shiness = shiness; }
	void setDetailScale(float scale) { m_detailScale = scale; }
	float getDetailScale() const { return m_detailScale; }
	bool haveDetail() const { return m_haveDetail; }

	Color3 getDiffuse() const;
	Color3 getSpecular() const;
	float getShiness() const;

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
	Texture * const * getTextures() const { return m_textures; }

	// parameter setting and getting
	void clearParameters();
	void addParameter(const FixedString &name, int num, const float *ptr);
	const FastParams *getParameters() const;

	// shader macro
	const ShaderMacro &getShaderMacro();

	void setTexMatrix(const Matrix4 &matrix);
	bool isTexAnim() const { return m_isTexAnim; }
	const Matrix4 *getTexMatrix() const { return &m_texMatrix; }

	static std::string normalizeKey(const std::string &name);

private:
	std::string m_name;
	MaterialDecl *m_decl;
	SortHint m_sortHint;

	ShaderMacro m_shaderMacro;

	Color3 m_diffuse;
	Color3 m_specular;
	Color3 m_emission;
	float m_shiness;
	float m_detailScale;

	const ShaderInfo *m_shaderInfo;
	FixedString m_shaderName;
	Texture *m_textures[MaterialTextureId::MaxType];
	FastParams *m_shaderParams;
	ConstBuffer *m_localUniforms;

	// tex anim etc...
	Matrix4 m_texMatrix;

	bool m_features[MAX_FEATURES];

	bool m_isTexAnim : 1;
	bool m_shaderMacroNeedRegen : 1;
	bool m_haveDetail : 1;

	// render state
	bool m_depthWrite : 1;
	bool m_depthTest : 1;
	bool m_twoSided : 1;
	bool m_wireframed : 1;
	BlendMode m_blendMode : 8;
};

inline void Material::setDiffuse(const Color3 &v)
{
	m_diffuse = v;
}

inline void Material::setSpecular(const Color3 &v)
{
	m_specular = v;
}


inline Color3 Material::getDiffuse() const
{
	return m_diffuse;
}

inline Color3 Material::getSpecular() const
{
	return m_specular;
}

inline float Material::getShiness() const
{
	return m_shiness;
}

inline void Material::setFeature(int index, bool enabled)
{
	AX_ASSERT(index >= 0 && index < MAX_FEATURES);
	if (m_features[index] != enabled) {
		m_shaderMacroNeedRegen = true;
		m_features[index] = enabled;
	}
}

inline bool Material::isFeatureEnabled(int index) const
{
	AX_ASSERT(index >= 0 && index < MAX_FEATURES);
	return m_features[index];
}

inline void Material::clearFeatures()
{
	m_shaderMacroNeedRegen = true;
	TypeZeroArray(m_features);
}

inline Texture *Material::getTexture(int sampler) const
{
	AX_ASSERT(sampler >= 0 && sampler < MaterialTextureId::MaxType);
	return m_textures[sampler];
}

inline void Material::setTexture(int sampler, Texture *tex)
{
	if (m_textures[sampler] == tex)
		return;

	m_shaderMacroNeedRegen = true;
	AX_ASSERT(sampler >= 0 && sampler < MaterialTextureId::MaxType);
	SafeDelete(m_textures[sampler]);
	m_textures[sampler] = tex;
}


AX_END_NAMESPACE

#endif // AX_MATERIAL_H
