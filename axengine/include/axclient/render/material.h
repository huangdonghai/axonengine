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
		Unknown,
#define AX_ENUM(x) x,
		AX_DECL_SURFACETYPE
#undef AX_ENUM
		// Dust, Metal, Sand, Wood, Grass, Snow, Glass, Water
	};
	AX_DECLARE_ENUM(SurfaceType);

	const char *toString() {
		switch (m_t) {
#define AX_ENUM(x) case x: return #x;
			AX_DECL_SURFACETYPE
#undef AX_ENUM
		}

		return "INVALID";
	}

	static SurfaceType fromString(const char *str) {
#define AX_ENUM(x) if (Strequ(str, #x)) { return x; }
		AX_DECL_SURFACETYPE
#undef AX_ENUM
			return Unknown;
	}
};

struct GlobalTextureId {
	enum Type {
		RtDepth, Rt0, Rt1, Rt2, Rt3,
		SceneColor, ShadowMap, ShadowMapCube,

		MaxType
	};
	AX_DECLARE_ENUM(GlobalTextureId);
};

struct MaterialTextureId {
	enum Type {
		// material sampler
		Diffuse, Normal, Specular, Opacit, Emission, Displacement, Env,

		// terrain sampler
		TerrainColor, TerrainNormal,

		Detail, Detail1, Detail2, Detail3, 
		DetailNormal, DetailNormal1, DetailNormal2, DetailNormal3, 
		LayerAlpha, LayerAlpha1, LayerAlpha2, LayerAlpha3,

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
		AX_ASSURE(item.offset + num < NUM_FLOATDATA);
		::memcpy(m_floatData+item.offset, data, num * sizeof(float));
		m_numItems++;
	}

public:
	enum { NUM_ITEMS = PRIMITIVECONST_COUNT, NUM_FLOATDATA = PRIMITIVECONST_COUNT * 4 };
	struct Item {
		int nameId;
		int offset;
		int count;
	};
	int m_numItems;
	Item m_items[NUM_ITEMS];
	float m_floatData[NUM_FLOATDATA];
};

class FastTextureParams
{
public:
	FastTextureParams() { m_numItems = 0; }
	~FastTextureParams() {}

	void clear() { m_numItems = 0; }
	void addSampler(MaterialTextureId id, phandle_t handle, SamplerDesc state)
	{
		AX_ASSURE(m_numItems < NUM_ITEMS);
		Item &item = m_items[m_numItems];
		item.id = id;
		item.handle = handle;
		item.samplerState = state;
		m_numItems++;
	}
public:
	enum { NUM_ITEMS = 16 };
	struct Item {
		MaterialTextureId id;
		phandle_t handle;
		SamplerDesc samplerState;
	};
	int m_numItems;
	Item m_items[NUM_ITEMS];
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

	Material *clone() const;

	const ShaderInfo *getShaderInfo() const { return m_shaderInfo; }
	const FixedString &getShaderName() const { return m_shaderName; }
	bool isWireframe() const;
	bool isPhysicsHelper() const;

	SortHint getSortHint() const { return m_sortHint; }

	void setTextureSet(const std::string &texname);

	void setDiffuse(const Color3 &v);
	void setSpecular(const Color3 &v);
	void setShiness(float shiness) { m_shiness = shiness; }
	void setDetailScale(int index, float value) { m_detailScale[index] = value; }
	void setDetailScale(Vector4 scale) { m_detailScale = scale; }
	Vector4 getDetailScale() const { return m_detailScale; }

	Color3 getDiffuse() const;
	Color3 getSpecular() const;
	float getShiness() const;

	// features and shader parameter
	void setFeature(int index, bool enabled);
	bool isFeatureEnabled(int index) const;
	void clearFeatures();

	// texture setting and getting
	Texture *getTexture(int sample) const;
	void setTexture(int sampler, Texture *tex);
	const FastTextureParams *getTextureParams() const { return &m_fastSamplerParams; }

	// parameter setting and getting
	void clearParameters();
	void addParameter(const FixedString &name, int num, const float *ptr);
	const FastParams *getParameters() const;

	// shader macro
	const MaterialMacro &getShaderMacro();

	void setTexMatrix(const Matrix4 &matrix);
	bool isTexAnim() const { return m_isTexAnim; }
	const Matrix4 *getTexMatrix() const { return &m_texMatrix; }

	static std::string normalizeKey(const std::string &name);

private:
	Material() {}
	std::string m_name;
	MaterialDecl *m_decl;
	SortHint m_sortHint;

	MaterialMacro m_shaderMacro;

	Color3 m_diffuse;
	Color3 m_specular;
	Color3 m_emission;
	float m_shiness;
	Vector4 m_detailScale;

	const ShaderInfo *m_shaderInfo;
	FixedString m_shaderName;
	Texture *m_textures[MaterialTextureId::MaxType];
	FastParams m_shaderParams;
	FastTextureParams m_fastSamplerParams;

	// tex animation etc...
	Matrix4 m_texMatrix;

	bool m_features[MAX_FEATURES];

	bool m_isTexAnim : 1;
	bool m_macroNeedRegen : 1;

	// render state
	bool m_depthWrite : 1;
	bool m_depthTest : 1;
	bool m_wireframed : 1;
	BlendMode m_blendMode : 4;
	RasterizerDesc::CullMode m_cullMode : 4;
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
		m_macroNeedRegen = true;
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
	m_macroNeedRegen = true;
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

	m_macroNeedRegen = true;
	AX_ASSERT(sampler >= 0 && sampler < MaterialTextureId::MaxType);
	SafeDelete(m_textures[sampler]);
	m_textures[sampler] = tex;
}


AX_END_NAMESPACE

#endif // AX_MATERIAL_H
