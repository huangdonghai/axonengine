/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_ENGINE_MATERIALFILE_H
#define AX_ENGINE_MATERIALFILE_H


#include "../../../data/shaders/shared.fxh"

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
	AX_ENUM(NUMBER_ALL)

struct SurfaceType {
	enum Type {
#define AX_ENUM(x) x,
	AX_DECL_SURFACETYPE
#undef AX_ENUM
// Dust, Metal, Sand, Wood, Grass, Snow, Glass, Water
	} t;
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
#define AX_ENUM(x) if (Strequ(str, #x)) {return x; }
		AX_DECL_SURFACETYPE
#undef AX_ENUM
		return 0;
	}
};

struct SamplerType {
	enum Type {
		// material sampler
		Diffuse, Normal, Specular, Detail, DetailNormal, Opacit, Emission, Displacement,
		Envmap, Custom1, Custom2, 

		// engine sampler
		TerrainColor, TerrainNormal, LayerAlpha,

		NUMBER_ALL
	} t;
	AX_DECLARE_ENUM(SamplerType);
};

class TextureDef {
public:
	TextureDef();
	String file;
	bool clamp;
	bool clampToEdge;
	bool filter;
};

inline TextureDef::TextureDef() : clamp(false), clampToEdge(false), filter(true) {}

typedef Sequence<float> FloatSeq;
typedef Dict<String, FloatSeq> ShaderParams;

class MaterialDecl
{
public:
	enum {
		MAX_FEATURES = 8,
		MAX_LITERALS = 8
	};

	enum CreationFlag {
		JustParse = 1,
		BlendDefault = 2,
		LightedDefault = 4
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
		DontDraw = 1,
		TwoSided = 2,
		Wireframed = 4,
		PhysicsHelper = 8,
	};

	typedef Flags_<Flag> Flags;

	bool tryLoad(const String &name);

	const String &getShaderName() { return m_shaderName; }
	TextureDef *getTextureDef(SamplerType maptype) { return m_textures[maptype]; }
	bool isWireframed() const { return m_flags.isSet(Wireframed); }
	bool isTwoSided() const { return m_flags.isSet(TwoSided); }
	Flags getFlags() const { return m_flags; }
	void setFlags(Flags flags) { m_flags = flags; }
	Rgba getDiffuse() const { return m_diffuse; }
	const bool *getFeatures() const { return m_features; }

	friend class MaterialDeclManager;
	bool isDefaulted() const { return this == ms_defaulted; }

	// management
	static MaterialDecl *load(const String &name);
	static void initManager();
	static void finalizeManager();

protected:
	MaterialDecl();
	virtual ~MaterialDecl();

	static FixedString normalizeKey(const String &name);

private:
	FixedString m_key;
	String m_shaderName;
	int m_shaderGenMask;
	Flags m_flags;
	SurfaceType m_surfaceType;
	TextureDef *m_textures[SamplerType::NUMBER_ALL];
	ShaderParams m_shaderParams;
	Rgba m_diffuse, m_specular, m_emission;
	float m_specExp, m_specLevel;
	float m_opacity;
	float m_detailScale;

	// features and literals
	bool m_features[MAX_FEATURES];
	int m_literals[MAX_LITERALS];

	// manager
	static MaterialDecl *ms_defaulted;
	typedef Dict<FixedString, MaterialDecl*> MaterialDeclDict;
	typedef DictSet<FixedString> FailureSet;
	static MaterialDeclDict ms_declDict;
	static FailureSet ms_failureSet;
};

AX_END_NAMESPACE

#endif // end guardian

