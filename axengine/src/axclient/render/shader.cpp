/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------

class ShaderMacro::ShaderMacroDef
{
public:
	struct Item {
		const char *name;
		int offset;
		int numbits;
		int mask;
	};

	ShaderMacroDef();

	void initGlobalMacros();
	void initMaterialMacros();

public:
	enum { MAX_ITEMS = 64};
	int m_numItems;
	Item m_items[MAX_ITEMS];
};

ShaderMacro::ShaderMacroDef::ShaderMacroDef()
{
	m_numItems = 0;
}

void ShaderMacro::ShaderMacroDef::initGlobalMacros()
{
	int bitoffset = 0;

#define AX_DECL_MACRO(m, s) \
	AX_RELEASE_ASSERT (bitoffset + s <= 31); \
	m_items[GlobalMacro::m].name = #m; \
	m_items[GlobalMacro::m].offset = bitoffset; \
	m_items[GlobalMacro::m].numbits = s; \
	m_items[GlobalMacro::m].mask = ~(((1LL<<(bitoffset+s)) - 1) - ((1LL<<bitoffset)-1)); \
	bitoffset += s; m_numItems++;

	AX_SHADERMACRO_DEFS
#undef AX_DECL_MACRO
}

void ShaderMacro::ShaderMacroDef::initMaterialMacros()
{
	int bitoffset = 0;

#define AX_DECL_MACRO(m, s) \
	AX_RELEASE_ASSERT (bitoffset + s <= 31); \
	m_items[MaterialMacro::m].name = #m; \
	m_items[MaterialMacro::m].offset = bitoffset; \
	m_items[MaterialMacro::m].numbits = s; \
	m_items[MaterialMacro::m].mask = ~(((1LL<<(bitoffset+s)) - 1) - ((1LL<<bitoffset)-1)); \
	bitoffset += s; m_numItems++;

	AX_MATERIALMACRO_DEFS
#undef AX_DECL_MACRO
}



ShaderMacro::ShaderMacroDef *ShaderMacro::ms_globalDefs = 0;
ShaderMacro::ShaderMacroDef *ShaderMacro::ms_materialDefs = 0;

ShaderMacro::ShaderMacro(Type type)
{
	if (!ms_globalDefs) {
		ms_globalDefs = new ShaderMacro::ShaderMacroDef();
		ms_globalDefs->initGlobalMacros();
		ms_materialDefs = new ShaderMacro::ShaderMacroDef();
		ms_materialDefs->initMaterialMacros();
	}

	if (type == kGlobalMacro)
		m_data = 0;
	else
		m_data = TYPEBIT;
}

ShaderMacro::ShaderMacro(const ShaderMacro &rhs)
{
	m_data = rhs.m_data;
}

ShaderMacro::~ShaderMacro()
{
//		TypeFree(m_data);
}

void ShaderMacro::clear()
{
	m_data &= TYPEBIT;
}

void ShaderMacro::setMacro(int f, int value)
{
	AX_ASSERT(f < def()->m_numItems);
	const ShaderMacroDef::Item &item = def()->m_items[f];

	AX_ASSERT(value >= 0 && value <(1<<item.numbits));

	m_data &= item.mask;
	m_data |= value << item.offset;
}

void ShaderMacro::setMacro(int f)
{
	setMacro(f, 1);
}

int ShaderMacro::getMacro(int f) const
{
	AX_ASSERT(f < def()->m_numItems);
	const ShaderMacroDef::Item &item = def()->m_items[f];

	return (m_data & ~item.mask) >> item.offset;
}

void ShaderMacro::resetMacro(int f)
{
	setMacro(f, 0);
}

bool ShaderMacro::isSet(int f) const
{
	return getMacro(f) != 0;
}

const char *ShaderMacro::getMacroName(int f) const
{
	AX_ASSERT(f <= def()->m_numItems);
	return def()->m_items[f].name;
}


StringSeq ShaderMacro::getCgDefine() const
{
	StringSeq result;
	char buf[64];

	result.reserve(def()->m_numItems);

	for (int i = 0; i < def()->m_numItems; i++) {
		int v = StringUtil::snprintf(buf, 64, "-D%s=%d", getMacroName(i), getMacro(i));
		AX_ASSERT(v < 64);
		result.push_back(buf);
	}
	return result;
}

StringPairSeq ShaderMacro::getDefines() const
{
	StringPairSeq result;

	char buf[64];

	result.resize(def()->m_numItems);

	for (int i = 0; i < def()->m_numItems; i++) {
		int v = StringUtil::snprintf(buf, 64, "%d", getMacro(i));
		AX_ASSERT(v < 64);

		result[i].first = getMacroName(i);
		result[i].second = buf;
	}
	return result;
}

#if 0
void ShaderMacro::mergeFrom(const ShaderMacro *from)
{
	if (this == from) {
		return;
	}
#ifdef _DEBUG
	for (int i = 0; i < NUM_FLAGS; i++) {
		Flag f = (Flag)i;
		int fv = from->getMacro(f);
		if (fv) {
			if (getMacro(f)) {
				Debugf("conflicted merge '%s'\n", getMacroName(i));
			}
			setMacro(f,fv);
		}
	}
#else
	for (int i = 0; i < ArraySize(m_data); i++) {
		m_data[i] = from->m_data[i];
	}
#endif
}
#endif

size_t ShaderMacro::hash() const
{
	return m_data;
}

bool ShaderMacro::operator==(const ShaderMacro &rhs) const
{
	return m_data == rhs.m_data;
}

std::string ShaderMacro::toString() const
{
	std::string result;
	StringUtil::sprintf(result, "%x", m_data);
	return result;
}

void ShaderMacro::fromString( const std::string &str )
{
	int v = sscanf(str.c_str(), "%x", &m_data);
	AX_ASSERT(v == 1);
}

const ShaderMacro::ShaderMacroDef * ShaderMacro::def() const
{
	if (isGlobal()) return ms_globalDefs;
	else return ms_materialDefs;
}

static inline int getSizeOfValueType(ConstBuffer::ValueType vt)
{
	switch (vt) {
	case ConstBuffer::vt_float:
		return 1 * sizeof(float);
	case ConstBuffer::vt_Vector2:
		return 2 * sizeof(float);
	case ConstBuffer::vt_Vector3:
		return 3 * sizeof(float);
	case ConstBuffer::vt_Vector4:
		return 4 * sizeof(float);
	case ConstBuffer::vt_Matrix3:
		return 3*3 * sizeof(float);
	case ConstBuffer::vt_Matrix:
		return 3*4 * sizeof(float);
	case ConstBuffer::vt_Matrix4:
		return 4*4 * sizeof(float);
	}

	AX_WRONGPLACE;
	return 0;
}

static inline bool isMatrix(ConstBuffer::ValueType vt)
{
	switch (vt) {
	case ConstBuffer::vt_empty:
	case ConstBuffer::vt_float:
	case ConstBuffer::vt_Vector2:
	case ConstBuffer::vt_Vector3:
	case ConstBuffer::vt_Vector4:
		return false;
	case ConstBuffer::vt_Matrix3:
	case ConstBuffer::vt_Matrix:
	case ConstBuffer::vt_Matrix4:
		return true;
	}

	AX_WRONGPLACE;
	return false;
}



ConstBuffer::ConstBuffer(Type type)
{
	m_type = type;

	m_byteSize = 0;
	if (type == SceneConst)
		initSceneConst();
	else if (type == InteractionConst)
		initInteractionConst();
	else
		AX_WRONGPLACE;

	m_data.resize(m_byteSize);
}

ConstBuffer::~ConstBuffer()
{
}

void ConstBuffer::addField(ValueType vt, Item name, int count, int reg)
{
	Field field;
	field.m_valueType = vt;
	field.m_name = name;
	field.m_arrayCount = count;
	field.m_byteSize = getSizeOfValueType(vt) * count;
	field.m_byteOffset = reg * 4 * sizeof(float);

	int size = field.m_byteOffset + field.m_byteSize;
	m_byteSize = std::max(size, m_byteSize);

	m_fields.push_back(field);
}

#if 0
ConstBuffer *ConstBuffer::clone() const
{
	ConstBuffer *cloned = new ConstBuffer();
	cloned->m_data = m_data;
	cloned->m_default = m_default;
	cloned->m_fields = m_fields;
	cloned->m_dirty = m_dirty;
	cloned->m_index = m_index;

	return cloned;
}
#endif

void ConstBuffer::initSceneConst()
{
	clear();

#define AX_ITEM(stype, atype, name, reg) addField(vt_##atype, name, 1, reg);
#define AX_ARRAY(stype, atype, name, n, reg) addField(vt_##atype, name, n, reg);
#include "../../../data/shaders/sceneconst.fxh"
#undef AX_ITEM
#undef AX_ARRAY
}

void ConstBuffer::initInteractionConst()
{
	clear();

#define AX_ITEM(stype, atype, name, reg) addField(vt_##atype, name, 1, reg);
#define AX_ARRAY(stype, atype, name, n, reg) addField(vt_##atype, name, n, reg);
#include "../../../data/shaders/interactionconst.fxh"
#undef AX_ITEM
#undef AX_ARRAY
}

void ConstBuffer::clear()
{
	m_data.clear();
	m_default.clear();
	m_fields.clear();
	m_dirty = true;
}

void ConstBuffer::setData(int bytes, const void *datap)
{
	AX_ASSERT(bytes <= m_data.size());
	::memcpy(&m_data[0], datap, bytes);
}


ConstBuffers::ConstBuffers()
{
	for (int i = 0; i < ConstBuffer::MaxType; i++) {
		m_buffers[i] = new ConstBuffer((ConstBuffer::Type)i);
		std::vector<ConstBuffer::Field> &fields = m_buffers[i]->m_fields;

		for (int j = 0; j < fields.size(); j++) {
			FieldLink *fieldLink = new FieldLink();
			fieldLink->m_buffer = m_buffers[i];
			fieldLink->m_field = &fields[j];
			m_fields[fields[j].m_name] = fieldLink;
		}
	}
}

ConstBuffers::~ConstBuffers()
{

}

void ConstBuffers::setField(Item fieldName, int bytes, const void *dataptr)
{
	static byte_t converted[sizeof(Matrix4)*4];
	FieldLink *fl = m_fields[fieldName];

	// check data size
	if (bytes != fl->m_field->m_byteSize)
		AX_WRONGPLACE;

	// check matrix
	if (g_renderDriverInfo.transposeMatrix) {
		AX_RELEASE_ASSERT(bytes <= sizeof(Matrix4)*4);

		switch (fl->m_field->m_valueType) {
		case ConstBuffer::vt_Matrix3:
			{
				int n = fl->m_field->m_arrayCount;
				const Matrix3 *src = reinterpret_cast<const Matrix3 *>(dataptr);
				Matrix3 *dst = reinterpret_cast<Matrix3 *>(converted);
				for (int i = 0; i < n; i++) {
					dst[i] = src[i].getTranspose();
				}
				dataptr = converted;
				break;
			}
		case ConstBuffer::vt_Matrix:
			{
				int n = fl->m_field->m_arrayCount;
				const Matrix *src = reinterpret_cast<const Matrix *>(dataptr);
				Matrix *dst = reinterpret_cast<Matrix *>(converted);
				for (int i = 0; i < n; i++) {
					dst[i] = src[i].getTranspose();
				}
				dataptr = converted;
				break;
			}
		case ConstBuffer::vt_Matrix4:
			{
				int n = fl->m_field->m_arrayCount;
				const Matrix4 *src = reinterpret_cast<const Matrix4 *>(dataptr);
				Matrix4 *dst = reinterpret_cast<Matrix4 *>(converted);
				for (int i = 0; i < n; i++) {
					dst[i] = src[i].getTranspose();
				}
				dataptr = converted;
				break;
			}
		}
	}

	// copy data
	if (::memcmp(&fl->m_buffer->m_data[fl->m_field->m_byteOffset], dataptr, bytes)) {
		memcpy(&fl->m_buffer->m_data[fl->m_field->m_byteOffset], dataptr, bytes);
		fl->m_buffer->m_dirty = true;
	}
}

void ConstBuffers::setData(Type type, int bytes, const void *datap)
{
	m_buffers[type]->setData(bytes, datap);
}

AX_END_NAMESPACE

