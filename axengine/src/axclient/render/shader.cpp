/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

#if 0
//--------------------------------------------------------------------------
// class UniformItem
//--------------------------------------------------------------------------

UniformItem::UniformItem()
{
	m_valueType = vt_empty;
	m_arraySize = 0;
	m_dataSize = 0;
	m_datap = 0;
	m_needUpdate = false;
}

UniformItem::UniformItem(const UniformItem &rhs)
{
	m_valueType = rhs.m_valueType;
	m_arraySize = rhs.m_arraySize;
	m_dataSize = rhs.m_dataSize;
	m_needUpdate = false;

	m_datap = Malloc(m_dataSize);
	m_name = rhs.m_name;		// ascii name
}

UniformItem::~UniformItem()
{
	Free(m_datap);
}

int UniformItem::calcValueSize(ValueType vt)
{
	switch (vt) {
		case vt_float: return sizeof(float);
		case vt_Vector2: return sizeof(Vector2);
		case vt_Vector3: return sizeof(Vector3);
		case vt_Vector4: return sizeof(Vector4);
		case vt_Matrix3: return sizeof(Matrix3);
		case vt_Matrix: return sizeof(Matrix);
		case vt_Matrix4: return sizeof(Matrix4);
		case vt_Texture: return sizeof(Texture*);
	}
	return 0;
}

void UniformItem::init(ValueType vt, const char *name, int vsreg, int psreg, int arraysize /*= 1 */)
{
	m_valueType = vt;
	m_arraySize = arraysize;
	m_dataSize = arraysize * calcValueSize(vt);

	m_vsregister = vsreg;
	m_psregister = psreg;

	m_datap = Malloc(m_dataSize);
	memset(m_datap, 0, m_dataSize);

	m_name = name;
}

static inline int calcNumRegister( UniformItem::ValueType vt )
{
	switch (vt) {
		case UniformItem::vt_float:
		case UniformItem::vt_Vector2:
		case UniformItem::vt_Vector3:
		case UniformItem::vt_Vector4:
			return 1;
		case UniformItem::vt_Matrix3:
			return 3;
		case UniformItem::vt_Matrix:
			return 3;
		case UniformItem::vt_Matrix4:
			return 4;
		case UniformItem::vt_Texture:
			return 0;
	}
	return 0;
}

//--------------------------------------------------------------------------
// class Uniforms
//--------------------------------------------------------------------------

Uniforms::Uniforms()
{
#define AX_ARRAY_UNIFORM(shadertype, axtype, name, num, vsreg, psreg) m_items[name].init(UniformItem::vt_##axtype, #name, vsreg, psreg, num);
#define AX_UNIFORM(fxtype, axtype, name, vsreg, psreg) m_items[name].init(UniformItem::vt_##axtype, #name, vsreg, psreg, 1);
#define AX_TEXTURE_UNIFORM(fxtype, name) m_items[name].init(UniformItem::vt_Texture, #name, 0, 0, 1);

	AX_UNIFORM_DEFS

#undef AX_TEXTURE_UNIFORM
#undef AX_UNIFORM
#undef AX_ARRAY_UNIFORM

	findRegisters();
}

Uniforms::~Uniforms()
{

}

UniformItem &Uniforms::getItem(int itemname)
{
	return m_items[itemname];
}

void Uniforms::findRegisters()
{
	TypeZeroArray(m_vsIsShared);
	TypeZeroArray(m_psIsShared);

	for (int i = 0; i < NUM_UNIFORM_ITEMS; i++) {
		UniformItem &item = m_items[i];

		if (!item.m_vsregister && !item.m_psregister)
			continue;

		int n = calcNumRegister(item.m_valueType) * item.m_arraySize;

		if (!n)
			continue;

		if (item.m_vsregister) {
			for (int j = 0; j < n; j++) {
				m_vsIsShared[item.m_vsregister+j] = true;
			}
		}

		if (item.m_psregister) {
			for (int j = 0; j < n; j++) {
				m_psIsShared[item.m_psregister+j] = true;
			}
		}
	}
}
#endif

//--------------------------------------------------------------------------
// class ShaderMacro
//--------------------------------------------------------------------------

class ShaderMacro::ShaderMacroDefs
{
public:
	typedef ShaderMacro::Flag Flag;
	struct Item {
		int intoffset;
		int offset;
		int numbits;
		int mask;
	};

	ShaderMacroDefs();

public:
	Item m_items[ShaderMacro::NUM_FLAGS];
	int m_size;
};

ShaderMacro::ShaderMacroDefs *ShaderMacro::m_defs = 0;

ShaderMacro::ShaderMacro()
{
	if (!m_defs) {
		m_defs = new ShaderMacro::ShaderMacroDefs();
		AX_ASSERT(ArraySize(m_data) >= size_t(m_defs->m_size));
	}

//		m_data = TypeAlloc<int>(m_defs->m_size);
	clear();
}

ShaderMacro::ShaderMacro(const ShaderMacro &rhs)
{
//		m_data = TypeAlloc<int>(m_defs->m_size);
	for (size_t i = 0; i < ArraySize(m_data); i++) {
		m_data[i] = rhs.m_data[i];
	}
}

ShaderMacro::~ShaderMacro()
{
//		TypeFree(m_data);
}

void ShaderMacro::clear()
{
	TypeZeroArray(m_data);
}

void ShaderMacro::setMacro(Flag f, int value)
{
	AX_ASSERT(f < NUM_FLAGS);
	const ShaderMacroDefs::Item &item = m_defs->m_items[f];

	AX_ASSERT(value >= 0 && value <(1<<item.numbits));

	m_data[item.intoffset] &= item.mask;
	m_data[item.intoffset] |= value << item.offset;
}

void ShaderMacro::setMacro(Flag f)
{
	setMacro(f, 1);
}

void ShaderMacro::setMacro(int f, int value)
{
	setMacro((Flag)f, value);
}

int ShaderMacro::getMacro(Flag f) const
{
	AX_ASSERT(f < NUM_FLAGS);
	const ShaderMacroDefs::Item &item = m_defs->m_items[f];

	return (m_data[item.intoffset] & ~item.mask) >> item.offset;
}

void ShaderMacro::resetMacro(Flag f)
{
	setMacro(f, 0);
}

bool ShaderMacro::isSet(Flag f) const
{
	return getMacro(f) != 0;
}

#define AX_DECL_MACRO(m, s) #m,
const char *ShaderMacro::getMacroName(int f) {
	AX_ASSERT(f <= NUM_FLAGS);

	static const char *macro_names[NUM_FLAGS] = {
		AX_SHADERMACRO_DEFS
	};

	return macro_names[f];

	Errorf("%s: error macro %d", __func__, f);

	return "";
}
#undef AX_DECL_MACRO


StringSeq ShaderMacro::getCgDefine() const
{
	StringSeq result;
	char buf[64];

	result.reserve(NUM_FLAGS);

	for (int i = 0; i < NUM_FLAGS; i++) {
		int v = StringUtil::snprintf(buf, 64, "-D%s=%d", getMacroName(i), getMacro((Flag)i));
		AX_ASSERT(v < 64);
		result.push_back(buf);
	}
	return result;
}

StringPairSeq ShaderMacro::getDefines() const
{
	StringPairSeq result;

	char buf[64];

	result.resize(NUM_FLAGS);

	for (int i = 0; i < NUM_FLAGS; i++) {
		int v = StringUtil::snprintf(buf, 64, "%d", getMacro((Flag)i));
		AX_ASSERT(v < 64);

		result[i].first = getMacroName(i);
		result[i].second = buf;
	}
	return result;
}

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

size_t ShaderMacro::hash() const
{
	size_t h = 0;
	for (size_t i = 0; i < ArraySize(m_data); i++) {
		hash_combine(h, m_data[i]);
	}

	return h;
}

void ShaderMacro::initDefs()
{
	m_defs = new ShaderMacroDefs();
}

bool ShaderMacro::operator==(const ShaderMacro &rhs) const
{
	for (size_t i = 0; i < ArraySize(m_data); i++) {
		if (m_data[i] != rhs.m_data[i]) {
			return false;
		}
	}
	return true;
}

std::string ShaderMacro::toString() const
{
	std::string result;
	StringUtil::sprintf(result, "%x %x %x %x", m_data[0], m_data[1], m_data[2], m_data[3]);
	return result;
}

void ShaderMacro::fromString( const std::string &str )
{
	int v = sscanf(str.c_str(), "%x %x %x %x", &m_data[0], &m_data[1], &m_data[2], &m_data[3]);
	AX_ASSERT(v == 4);
}

ShaderMacro::ShaderMacroDefs::ShaderMacroDefs()
{
	int intoffset = 0;
	int bitoffset = 0;

#define AX_DECL_MACRO(m, s) \
	if (bitoffset + s > 32) { \
		intoffset++; \
		bitoffset = 0; \
	} \
\
	m_items[ShaderMacro::m].intoffset = intoffset; \
	m_items[ShaderMacro::m].offset = bitoffset; \
	m_items[ShaderMacro::m].numbits = s; \
	m_items[ShaderMacro::m].mask = ~(((1LL<<(bitoffset+s)) - 1) - ((1LL<<bitoffset)-1)); \
	bitoffset += s;						

	AX_SHADERMACRO_DEFS
#undef AX_DECL_MACRO

		m_size = intoffset + 1;
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

void ConstBuffer::addField(const Field &field)
{
	m_fields.push_back(field);
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
	FieldLink *fl = m_fields[fieldName];

	// check data size
	if (bytes != fl->m_field->m_byteSize)
		AX_WRONGPLACE;

	// check matrix
	if (g_renderDriverInfo.transposeMatrix) {
		switch (fl->m_field->m_valueType) {
		case ConstBuffer::vt_Matrix3:
			{
				int n = fl->m_field->m_arrayCount;
				const Matrix3 *src = reinterpret_cast<const Matrix3 *>(dataptr);
				Matrix3 *dst = reinterpret_cast<Matrix3 *>(&fl->m_buffer->m_data[fl->m_field->m_byteOffset]);
				for (int i = 0; i < n; i++) {
					dst[i] = src[i].getTranspose();
				}
				return;
			}
		case ConstBuffer::vt_Matrix:
			{
				int n = fl->m_field->m_arrayCount;
				const Matrix *src = reinterpret_cast<const Matrix *>(dataptr);
				Matrix *dst = reinterpret_cast<Matrix *>(&fl->m_buffer->m_data[fl->m_field->m_byteOffset]);
				for (int i = 0; i < n; i++) {
					dst[i] = src[i].getTranspose();
				}
				return;
			}
		case ConstBuffer::vt_Matrix4:
			{
				int n = fl->m_field->m_arrayCount;
				const Matrix4 *src = reinterpret_cast<const Matrix4 *>(dataptr);
				Matrix4 *dst = reinterpret_cast<Matrix4 *>(&fl->m_buffer->m_data[fl->m_field->m_byteOffset]);
				for (int i = 0; i < n; i++) {
					dst[i] = src[i].getTranspose();
				}
				return;
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

