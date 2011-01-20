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

#if 0
class UniformData
{
public:
	enum DataType {
		kFloat, kInteger, kSampler
	};

	UniformData()
	{
		m_dataType = kFloat;
		m_count = 0;
		m_floatValue = 0;
	}

	~UniformData()
	{
		if (m_dataType != kSampler) {
			SafeDeleteArray(m_floatValue);
		}
	}

	void setFloat(int count, float *value)
	{
		m_dataType = kFloat;
		m_count = count;
		m_floatValue = new float[count];
		memcpy(m_floatValue, value, count * sizeof(float));
	}

	void setInteger(int count, int *value)
	{
		m_dataType = kInteger;
		m_count = count;
		m_intValue = new int[count];
		memcpy(m_intValue, value, count * sizeof(int));
	}

	void setTexture(Handle *phandle)
	{
		m_dataType = kSampler;
		m_count = 1;
		m_textureHandle = phandle;
	}

private:
	DataType m_dataType;

	int m_count;
	union {
		float *m_floatValue;
		int *m_intValue;
		Handle *m_textureHandle;
	};
};
#endif

class ConstBuffer
{
public:
	friend class ConstBuffers;

	enum Item {
#define AX_ITEM(stype, atype, name, reg) name,
#define AX_ARRAY(stype, atype, name, n, reg) name,
#include "../../../data/shaders/sceneconst.fxh"
#include "../../../data/shaders/interactionconst.fxh"
#undef AX_ITEM
#undef AX_ARRAY

		MaxItem
	};

	enum Type {
		SceneConst,
		InteractionConst,
		MaxType
	};

	enum ValueType {
		vt_empty, vt_float, vt_Vector2, vt_Vector3, vt_Matrix3, vt_Vector4, vt_Matrix, vt_Matrix4
	};

	struct Field {
		ValueType m_valueType;
		Item m_name;
		int m_arrayCount;
		int m_byteOffset;
		int m_byteSize;
	};

	ConstBuffer(Type type);
	~ConstBuffer();

	Type getType() const { return m_type; }
	int getByteSize() const { return m_data.size(); }
	const void *getDataPointer() const { return &m_data[0]; }
	bool isDirty() const { return m_dirty; }
	void clearDirty() { m_dirty = false; }

	void setData(int bytes, const void *datap);
	void setFieldData(const FixedString &fieldName, int bytes, const void *datap);
	const float *getFieldPointer(const FixedString &fieldName) const;

	void addField(const Field &field);

protected:
	void initSceneConst();
	void initInteractionConst();

	void clear();
	void addField(ValueType vt, Item name, int count, int reg);

private:
	Type m_type;
	bool m_dirty;
	std::vector<Field> m_fields;
	int m_byteSize;
	std::vector<byte_t> m_data;
	std::vector<byte_t> m_default;
};

class ConstBuffers
{
public:
	friend class RenderContext;

	typedef ConstBuffer::Type Type;
	typedef ConstBuffer::Item Item;

	ConstBuffers();
	~ConstBuffers();

	void setData(Type type, int bytes, const void *datap);
	void setField(Item fieldName, int bytes, const void *dataptr);

private:
	class FieldLink
	{
	public:
		ConstBuffer *m_buffer;
		ConstBuffer::Field *m_field;
	};

	ConstBuffer *m_buffers[ConstBuffer::MaxType];
	FieldLink *m_fields[ConstBuffer::MaxItem];
};


//-------------------------------------------------------------------------
#if 0
class UniformItem
{
public:
	enum ValueType {
		vt_empty, vt_float, vt_Vector2, vt_Vector3, vt_Matrix3, vt_Vector4, vt_Matrix, vt_Matrix4, vt_Texture
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

//-------------------------------------------------------------------------

class Uniforms
{
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

//-------------------------------------------------------------------------

struct ShaderQuality {
	enum Type {
		Low = SHADER::ShaderQuality_low,
		Middle = SHADER::ShaderQuality_middle,
		High = SHADER::ShaderQuality_high
	} t;
	AX_DECLARE_ENUM(ShaderQuality);
};
#endif

//-------------------------------------------------------------------------

class ShaderMacro
{
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

	std::string toString() const;
	void fromString(const std::string &str);

	static void initDefs();
	static const char *getMacroName(int f);

private:
	class ShaderMacroDefs;
	static ShaderMacroDefs *m_defs;
	int m_data[1];
};

//-------------------------------------------------------------------------

struct Technique {
	enum Type {
		GeoFill, ShadowGen, Main, Layer, Glow, Reflection, MaxType
	};

	AX_DECLARE_ENUM(Technique);

	std::string toString();
};

//-------------------------------------------------------------------------

inline std::string Technique::toString()
{
	std::string result("Technique");

#define ENUMDECL(t) case t: result += #t; break;
	switch (t) {
	ENUMDECL(GeoFill)
	ENUMDECL(ShadowGen)
	ENUMDECL(Main)
	ENUMDECL(Layer)
	}
#undef ENUMDECL
	return result;
}

//-------------------------------------------------------------------------

class ShaderInfo
{
public:
	bool m_needReflection;
	bool m_needSceneColor;
	bool m_haveTechnique[Technique::MaxType];
};

typedef Dict<FixedString, ShaderInfo*> ShaderInfoDict;

//--------------------------------------------------------------------------
#if 0
class Shader : public RefObject
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

	// need to implement
	virtual bool doInit(const String &name, const ShaderMacro &macro) = 0;
	virtual bool isDepthWrite() const = 0;
	virtual bool haveTextureTarget() const = 0;
	virtual int getNumSampler() const = 0;
	virtual SamplerInfo *getSamplerAnno(int index) const = 0;
	virtual int getNumTweakable() const = 0;
	virtual ParameterInfo *getTweakableDef(int index) = 0;
	virtual SortHint getSortHint() const = 0;
	virtual bool haveTechnique(Technique tech) const = 0;
	virtual const ShaderInfo *getShaderInfo() const = 0;
};
#endif
//--------------------------------------------------------------------------
#if 0
class ShaderManager
{
public:
	virtual Shader *findShader(const String &name, const ShaderMacro &macro) = 0;
	virtual Shader *findShader(const FixedString &nameId, const ShaderMacro &macro) = 0;
	virtual void saveShaderCache(const String &name = String()) = 0;
	virtual void applyShaderCache(const String &name = String()) = 0;
};
#endif

AX_END_NAMESPACE

AX_DECLARE_HASH_FUNCTION(ShaderMacro);

#endif // end guardian

