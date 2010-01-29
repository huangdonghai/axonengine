/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_SHADER_H
#define AX_RENDER_SHADER_H

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class UniformItem
//--------------------------------------------------------------------------

class UniformItem
{
public:
	enum ValueType {
		vt_empty, vt_float, vt_Vector2, vt_Vector3, vt_Matrix3, vt_Vector4, vt_AffineMat, vt_Matrix4, vt_Texture
	};

	UniformItem();
	UniformItem(const UniformItem &rhs);
	virtual ~UniformItem();

	void init(ValueType vt, const char *name, int vsreg=0, int psreg=0, int arraysize = 1);

	const char *getName() const { return m_name; }

	template< class Q >
	void set(const Q &q) {
		AX_ASSERT(sizeof(q) <= m_dataSize);
		if (memcmp(m_datap, &q, m_dataSize) == 0)
			return;

		::memcpy(m_datap, &q, sizeof(q));
		m_needUpdate = true;
	}

	template< class Q >
	const Q &get() const {
		return *(Q*)(data);
	}

	void setData(const void *p) { memcpy(m_datap,p,m_dataSize); }
	void *getDataP() const { return m_datap; }

protected:
	static int calcValueSize(ValueType vt);

public:
	ValueType m_valueType : 8;
	int m_arraySize : 8;
	int m_dataSize : 16;

	int m_vsregister;
	int m_psregister;
	bool m_needUpdate;

	void *m_datap;
	const char *m_name;		// ascii name
};

//--------------------------------------------------------------------------
// class Uniforms
//--------------------------------------------------------------------------

class Uniforms {
public:
	enum ItemName {
#define AX_ARRAY_UNIFORM(shadertype, axtype, name, num, start, end) name,
#define AX_UNIFORM(fxtype, axtype, name, s, e) name,
#define AX_TEXTURE_UNIFORM(fxtype, name) name,

		AX_UNIFORM_DEFS

#undef AX_TEXTURE_UNIFORM
#undef AX_UNIFORM
#undef AX_ARRAY_UNIFORM

		NUM_UNIFORM_ITEMS
	};

	enum { MAX_REGISTER = 1024 };

	Uniforms();
	~Uniforms();

	UniformItem &getItem(int itemname);

	bool isVsregisterShared(int n) const {
		if (n < 0 || n >= MAX_REGISTER)
			return false;

		return m_vsIsShared[n];
	}

	bool isPsregisterShared(int n) const {
		if (n < 0 || n >= MAX_REGISTER)
			return false;

		return m_psIsShared[n];
	}

protected:
	void findRegisters();

private:
	UniformItem m_items[NUM_UNIFORM_ITEMS];
	bool m_vsIsShared[MAX_REGISTER];
	bool m_psIsShared[MAX_REGISTER];
};

//--------------------------------------------------------------------------
// struct ShaderQuality
//--------------------------------------------------------------------------

struct ShaderQuality {
	enum Type {
		Low = SHADER::ShaderQuality_low,
		Middle = SHADER::ShaderQuality_middle,
		High = SHADER::ShaderQuality_high
	} t;
	AX_DECLARE_ENUM(ShaderQuality);
};

//--------------------------------------------------------------------------
// class ShaderMacro
//--------------------------------------------------------------------------

class ShaderMacro {
public:
	enum Flag {
#define AX_DECL_MACRO(m, s) m, 
		AX_SHADERMACRO_DEFS
#undef AX_DECL_MACRO

		NUM_FLAGS,
	};

	enum { VERSION = SHADER::SHADERMACRO_VERSION };

	ShaderMacro();
	ShaderMacro(const ShaderMacro &rhs);
	~ShaderMacro();

	void clear();

	void setMacro(int f, int value);
	void setMacro(Flag f, int value);
	void setMacro(Flag f);
	int getMacro(Flag f) const;
	void resetMacro(Flag f);
	bool isSet(Flag f) const;

	StringSeq getCgDefine() const;
	StringPairSeq getDefines() const;

	void mergeFrom(const ShaderMacro *from);

	// for hash_map use
	size_t hash() const;
	bool operator==(const ShaderMacro &rhs) const;

	String toString() const;
	void fromString(const String &str);

	static void initDefs();
	static const char *getMacroName(int f);

private:
	class ShaderMacroDefs;
	static ShaderMacroDefs *m_defs;
	int m_data[4];
};


// shadername_shadergenflags_systemflags_vertexdeform_texgen_numlights_lightsflags
enum BlendFactor {
	BlendFactor_zero,
	BlendFactor_one,
	BlendFactor_srcColor,
	BlendFactor_oneMinusSrcColor,
	BlendFactor_dstColor,
	BlendFactor_oneMinusDstColor,
	BlendFactor_srcAlpha,
	BlendFactor_oneMinusSrcAlpha,
	BlendFactor_srcAlphaSaturate
};

enum StencilOp {
	StencilOp_keep = 1,
	StencilOp_zero = 2,
	StencilOp_replace = 3,
	StencilOp_incrementSat = 4,
	StencilOp_decrementSat = 5,
	StencilOp_invert = 6,
	StencilOp_increment = 7,
	StencilOp_decrement = 8
};

enum CompareFunc {
	CompareFunc_disable = 0,	// disabled depth test, alpha test or stencil test
	CompareFunc_never = 1,
	CompareFunc_less = 2,
	CompareFunc_equal = 3,
	CompareFunc_lessEqual = 4,
	CompareFunc_greater = 5,
	CompareFunc_notEqual = 6,
	CompareFunc_greaterEqual= 7,
	CompareFunc_always = 8
};
enum CullMode {
	CullMode_front,
	CullMode_back,
	CullMode_none
};

enum BlendMode {
	BlendMode_replace = 0,		// another name for StageBlend_none
	BlendMode_add,
	BlendMode_blend,
	BlendMode_filter,
	BlendMode_terrain,			// first layer use replace, another pass use
	// blend_add
};

struct Technique {
	enum Type {
		Zpass, ShadowGen, Main, Layer, Glow, Reflection, Number
	} t;

	AX_DECLARE_ENUM(Technique);

	String toString();
};

inline String Technique::toString() {
	String result("Technique");

#define ENUMDECL(t) case t: result += #t; break;
	switch (t) {
	ENUMDECL(Zpass)
	ENUMDECL(ShadowGen)
	ENUMDECL(Main)
	ENUMDECL(Layer)
	}
#undef ENUMDECL
	return result;
}

class ShaderAnno {
public:
};

// render to texture annotations
class SamplerAnno {
public:
	enum RenderType {
		None,
		Reflection,
		SceneColor,
	};

	// default c-tor
	SamplerAnno() {
		m_renderType = None;
		m_width = 512;
		m_height = 512;
		m_needClearColor = true;
		m_needClearDepth = true;
	}

	RenderType m_renderType;
	String m_texName;
	int m_width, m_height;

	bool m_needClearColor;
	bool m_needClearDepth;
	bool m_needHdr;
};
typedef Sequence<SamplerAnno> SamplerAnnos;

class ParameterAnno {
public:
	enum WidgetType {
		kNone, kSlider, kNumeric, kColor
	};
	bool m_uiTweakable;
	String m_uiName;
	String m_uiHelp;
	String m_uiWidget;

	// for slider
	float m_uiMin, m_uiMax, m_uiStep;

	// for numeric and slider
	float m_defaultFloat;

	// for color
	Vector3 m_defaultColor;
};
typedef Sequence<ParameterAnno> ParameterAnnos;

class ShaderInfo
{
public:
	enum {
		MAX_FEATURES = 8,
		MAX_LITERALS = 8
	};

	enum SortHint {
		SortHint_opacit = SHADER::SortHint_opacit,
		SortHint_decal = SHADER::SortHint_decal,
		SortHint_underWater = SHADER::SortHint_underWater,
		SortHint_water = SHADER::SortHint_water,
		SortHint_aboveWater = SHADER::SortHint_aboveWater
	};

	SortHint m_sortHint;
	bool m_haveTextureTarget;
	SamplerAnnos m_samplerAnnos;
	ParameterAnnos m_parameterAnnos;
};

//--------------------------------------------------------------------------
// class Shader
//--------------------------------------------------------------------------

class Shader : public RefObject {
public:
	enum {
		MAX_FEATURES = 8,
		MAX_LITERALS = 8
	};

	enum SortHint {
		SortHint_opacit = SHADER::SortHint_opacit,
		SortHint_decal = SHADER::SortHint_decal,
		SortHint_underWater = SHADER::SortHint_underWater,
		SortHint_water = SHADER::SortHint_water,
		SortHint_aboveWater = SHADER::SortHint_aboveWater
	};

	// need to implement
	virtual bool doInit(const String &name, const ShaderMacro &macro) = 0;
	virtual bool isDepthWrite() const = 0;
	virtual bool haveTextureTarget() const = 0;
	virtual int getNumSampler() const = 0;
	virtual SamplerAnno *getSamplerAnno(int index) const = 0;
	virtual int getNumTweakable() const = 0;
	virtual ParameterAnno *getTweakableDef(int index) = 0;
	virtual SortHint getSortHint() const = 0;
	virtual bool haveTechnique(Technique tech) const = 0;
};

//--------------------------------------------------------------------------
// class ShaderManager
//--------------------------------------------------------------------------

class ShaderManager {
public:
	virtual Shader *findShader(const String &name, const ShaderMacro &macro) = 0;
	virtual Shader *findShader(const FixedString &nameId, const ShaderMacro &macro) = 0;
	virtual void saveShaderCache(const String &name = String()) = 0;
	virtual void applyShaderCache(const String &name = String()) = 0;
};

AX_END_NAMESPACE

namespace std { namespace tr1 {
	template<>
	struct hash<AX_NAMESPACE::ShaderMacro> {
		size_t operator()(const AX_NAMESPACE::ShaderMacro &sm) const { return sm.hash(); }
	};
}}

#endif // end guardian

