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

class ConstBuffers;
class ConstBuffer
{
	friend class ConstBuffers;

public:
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

	enum { PrimitiveConst = 2 };

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

//-------------------------------------------------------------------------

class ConstBuffers
{
	friend class RenderContext;

public:
	typedef ConstBuffer::Type Type;
	typedef ConstBuffer::Item Item;

	ConstBuffers();
	~ConstBuffers();

	void setData(Type type, int bytes, const void *datap);
	void setField(Item fieldName, int bytes, const void *dataptr);

	ConstBuffer *getBuffer(int type) const { return m_buffers[type]; }

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

class ShaderMacro
{
public:
	enum Type {
		kGlobalMacro, kMaterialMacro
	};

	enum { VERSION = SHADER::SHADERMACRO_VERSION };

	ShaderMacro(Type type);
	ShaderMacro(const ShaderMacro &rhs);
	~ShaderMacro();

	bool isGlobal() const { return m_data >= 0; }

	void clear();

	void setMacro(int f, int value);
	void setMacro(int f);
	int getMacro(int f) const;
	void resetMacro(int f);
	bool isSet(int f) const;

	StringSeq getCgDefine() const;
	StringPairSeq getDefines() const;

	// for hash_map use
	int id() const { return m_data; }
	size_t hash() const;
	bool operator==(const ShaderMacro &rhs) const;

	std::string toString() const;
	void fromString(const std::string &str);

	const char *getMacroName(int f) const;

protected:
	class ShaderMacroDef;
	const ShaderMacroDef *def() const;

private:
	static ShaderMacroDef *ms_globalDefs;
	static ShaderMacroDef *ms_materialDefs;
	enum { TYPEBIT = 0x80000000, DATABITS = 0x7fffffff };
	int m_data;
};

class GlobalMacro : public ShaderMacro
{
public:
	enum Flag {
#define AX_DECL_MACRO(m, s) m, 
		AX_SHADERMACRO_DEFS
#undef AX_DECL_MACRO

		NUM_FLAGS,
	};

	GlobalMacro() : ShaderMacro(kGlobalMacro) {}
};

class MaterialMacro : public ShaderMacro
{
public:
	enum Flag {
#define AX_DECL_MACRO(m, s) m, 
		AX_MATERIALMACRO_DEFS
#undef AX_DECL_MACRO

		NUM_FLAGS,
	};

	MaterialMacro() : ShaderMacro(kMaterialMacro) {}
};

//-------------------------------------------------------------------------

struct Technique {
	enum Type {
		GeoFill, ShadowGen, Main, MaxType
	};

	AX_DECLARE_ENUM(Technique);

	const char *toString();
};

//-------------------------------------------------------------------------

inline const char *Technique::toString()
{
#define ENUMDECL(t) case t: return #t; break;
	switch (m_t) {
	ENUMDECL(GeoFill)
	ENUMDECL(ShadowGen)
	ENUMDECL(Main)
	}
#undef ENUMDECL
	return "";
}

//-------------------------------------------------------------------------

class ShaderInfo
{
public:
	bool m_needReflection;
	bool m_needSceneColor;
	bool m_haveTechnique[Technique::MaxType];
};

struct ShaderKey {
	int nameId;
	int gm;
	int mm;

	ShaderKey(const FixedString &_nameId, const GlobalMacro &_gm, const MaterialMacro &_mm)
	{
		nameId = _nameId.id();
		gm = _gm.id();
		mm = _mm.id();
	}
	operator size_t() const { size_t result = nameId; hash_combine(result, gm); hash_combine(result, mm); return result; }
};


AX_END_NAMESPACE

AX_DECLARE_HASH_FUNCTION(GlobalMacro);
AX_DECLARE_HASH_FUNCTION(MaterialMacro);

#endif // end guardian

