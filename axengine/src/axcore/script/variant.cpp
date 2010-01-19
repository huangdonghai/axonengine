#include "script_p.h"
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>

AX_BEGIN_NAMESPACE

using namespace boost;

typedef mpl::vector<void,bool,int,float,String,Object *,Vector3,Color3,Point,Rect,Matrix, LuaTable> VariantTL;

typedef bool (*castFunc)(const void *from, void *to);

template <class T, class Q, class Can>
struct CastHelper_ {
	static bool CastFunc(const void *from, void *to)
	{
		if (std::tr1::is_convertible<T,Q>::value) {
			*(Q *)to = *(const T *)from;
			return true;
		}

		return false;
	}
};

template <class T, class Q>
struct CastHelper_<T, Q, mpl::int_<0>> {
	static bool CastFunc(const void *from, void *to)
	{
		return false;
	}
};


class TypeHandler_Generic : public Variant::TypeHandler
{
public:
	virtual bool canCast(Variant::TypeId toType)
	{
		return m_canCasts[toType];
	}

	virtual bool rawCast(const void *fromData, Variant::TypeId toType, void *toData)
	{
		return m_castFuncs[toType](fromData, toData);
	}

protected:
	bool m_canCasts[Variant::kMaxType];
	castFunc m_castFuncs[Variant::kMaxType];
};

#define VT(x) mpl::at<VariantTL, mpl::int_<x>>::type
#define VTI(x) GetVariantType_<VT(x)>()
#define CANCAST(x) m_canCasts[VTI(x)] = std::tr1::is_convertible<T, VT(x)>::value
#define CASTFUNC(x) m_castFuncs[VTI(x)] = &CastHelper_<T, VT(x), mpl::int_<std::tr1::is_convertible<T, VT(x)>::value>>::CastFunc

template <class T>
class TypeHandler_ : public TypeHandler_Generic
{
public:
	TypeHandler_()
	{
		m_dataSize = sizeof(T);
		TypeZeroArray(m_canCasts);
		TypeZeroArray(m_castFuncs);

		CANCAST(0);
		CANCAST(1);
		CANCAST(2);
		CANCAST(3);
		CANCAST(4);
		CANCAST(5);
		CANCAST(6);
		CANCAST(7);
		CANCAST(8);
		CANCAST(9);
		CANCAST(10);
		CANCAST(11);

		CASTFUNC(0);
		CASTFUNC(1);
		CASTFUNC(2);
		CASTFUNC(3);
		CASTFUNC(4);
		CASTFUNC(5);
		CASTFUNC(6);
		CASTFUNC(7);
		CASTFUNC(8);
		CASTFUNC(9);
		CASTFUNC(10);
		CASTFUNC(11);
	}

	void construct(void *ptr, const void *copyfrom)
	{
		new (ptr) T(*(const T *)copyfrom);
	}

	void construct(void *ptr)
	{
		new (ptr) T();
	}

	void destruct(void *ptr)
	{
		((T *)(ptr))->~T();
	}
};

#undef CANCAST
#undef CASTFUNC

#define NEW_TH(x) new TypeHandler_<mpl::at<VariantTL, mpl::int_<x> >::type>
static Variant::TypeHandler *s_typeHandlers[Variant::kMaxType] = {
	0,
	NEW_TH(1),
	NEW_TH(2),
	NEW_TH(3),
	NEW_TH(4),
	NEW_TH(5),
	NEW_TH(6),
	NEW_TH(7),
	NEW_TH(8),
	NEW_TH(9),
	NEW_TH(10),
	NEW_TH(11),
	ScriptValue::getTypeHandler()
};
AX_STATIC_ASSERT(13==Variant::kMaxType);
#undef NEW_TH

String Variant::toString() const
{
	String result;
	switch (m_type) {
	case kVoid:
		break;
	case kBool:
		StringUtil::sprintf(result, "%d", bool(*this));
		break;
	case kInt:
		StringUtil::sprintf(result, "%d", int(*this));
		break;
	case kFloat:
		StringUtil::sprintf(result, "%f", float(*this));
		break;
	case kString:
		return ref<String>();
	case kObject:
		break;
	case kTable:
		break;
	case kVector3:
		{
			return ref<Vector3>().toString();
		}
	case kColor3:
		{
			return ref<Color3>().toString();
		}
	case kPoint:
		{
			return ref<Point>().toString();
		}
	case kRect:
		{
			return ref<Rect>().toString();
		}
	case kMatrix:
		{
			return ref<Matrix>().toString();
		}
	default:
		AX_NO_DEFAULT;
	}
	return result;
}

void Variant::fromString(TypeId t, const char *str)
{
	clear();

	switch (t) {
	case kVoid:
		break;
	case kBool:
		*this = (atoi(str) ? true : false);
		break;
	case kInt:
		*this = (atoi(str));
		break;
	case kFloat:
		*this = float(atof(str));
		break;
	case kString:
		*this = (str);
		break;
	case kObject:
		break;
	case kTable:
		break;
	case kVector3:
		{
			Vector3 v;
			v.fromString(str);
			*this = (v);
			break;
		}
	case kColor3:
		{
			Color3 v;
			v.fromString(str);
			*this = (v);
			break;
		}
	case kPoint:
		{
			Point v;
			v.fromString(str);
			*this = (v);
			break;
		}
	case kRect:
		{
			Rect v; v.fromString(str);
			*this = (v);
			break;
		}
	case kMatrix:
		{
			Matrix v;
			v.fromString(str);
			*this = v;
			break;
		}
	default:
		AX_NO_DEFAULT;
	}
}

int Variant::getTypeSize(TypeId t)
{
	return s_typeHandlers[t]->m_dataSize;
}

bool Variant::canCast(TypeId fromType, TypeId toType)
{
	if (fromType == toType)
		return true;

	return s_typeHandlers[fromType]->canCast(toType);
}

bool Variant::rawCast(TypeId fromType, const void *fromData, TypeId toType, void *toData)
{
	if (!canCast(fromType, toType))
		return false;

	return s_typeHandlers[fromType]->rawCast(fromData, toType, toData);
}

void Variant::init(Variant::TypeId t, const void *fromData)
{
	m_type = t;
	TypeHandler *h = s_typeHandlers[t];

	void *toData = 0;
	if (h->m_dataSize > MINIBUF_SIZE) {
		m_isMinibuf = false;
		toData = m_voidstar = Malloc(h->m_dataSize);
	} else {
		m_isMinibuf = true;
		toData = m_minibuf;
	}

	if (fromData)
		h->construct(toData, fromData);
	else
		h->construct(toData);
}

Variant::TypeHandler * Variant::getHandler() const
{
	AX_ASSERT(m_type < kMaxType);
	return s_typeHandlers[m_type];
}

void Variant::clear()
{
	TypeHandler *handler = getHandler();
	if (!handler)
		return;

	void *data = getPointer();
	handler->destruct(data);

	if (!m_isMinibuf) {
		Free(m_voidstar);
	}

	m_type = kVoid;
}

Variant::TypeHandler * Variant::getHandler(Variant::TypeId typeId)
{
	AX_ASSERT(typeId >= 0 && typeId < kMaxType);
	return s_typeHandlers[typeId];
}

//--------------------------------------------------------------------------
// class LuaTable
//--------------------------------------------------------------------------

LuaTable::LuaTable() : m_index(-10000), m_isReading(false), m_isIteratoring(false), m_stackTop(0)
{}


LuaTable::LuaTable( const LuaTable &rhs ) : m_index(rhs.m_index), m_isReading(false), m_isIteratoring(false), m_stackTop(0)
{}

LuaTable::LuaTable(int index)
	: m_index(index)
	, m_isReading(false)
	, m_isIteratoring(false)
	, m_stackTop(0)
{
	if (m_index < 0) {
		const_cast<int&>(m_index) = lua_gettop(L) + m_index + 1;
	}
}

void LuaTable::beginRead() const
{
	AX_ASSURE(!m_isReading && !m_isIteratoring);
	m_isReading = true;
	m_stackTop = lua_gettop(L);
}

void LuaTable::endRead() const
{
	AX_ASSURE(m_isReading);
	lua_settop(L,m_stackTop);
}

Variant LuaTable::get(const String &n) const
{
	AX_ASSURE(m_isReading);

	lua_pushstring(L, n.c_str());
	lua_rawget(L, m_index);
	Variant result = xReadStack(L, -1);

	if (result.getTypeId() != Variant::kTable) {
		lua_pop(L, 1);
	}
	return result;
}

int LuaTable::getLength() const
{
	AX_ASSURE(m_isReading);
	return lua_objlen(L,m_index);
}

Variant LuaTable::get(int n) const
{
	AX_ASSURE(m_isReading);
	lua_rawgeti(L, m_index, n+1);
	return xReadStack(L, -1);
}

void LuaTable::set(const String &n, const Variant &v)
{
	lua_pushstring(L, n.c_str());
	xPushStack(L, v);
	lua_settable(L, m_index);
}

Vector3 LuaTable::toVector3() const
{
	Vector3 result;
	beginRead();
	result.x = get("x");
	result.y = get("y");
	result.z = get("z");
	endRead();
	return result;
}

Color3 LuaTable::toColor() const
{
	Color3 result;
	beginRead();
	result.r = get("r");
	result.g = get("g");
	result.b = get("b");
	endRead();
	return result;
}

Point LuaTable::toPoint() const
{
	Point result;
	beginRead();
	result.x = get("x");
	result.y = get("y");
	endRead();
	return result;
}

Rect LuaTable::toRect() const
{
	Rect result;
	beginRead();
	result.x = get("x");
	result.y = get("y");
	result.width = get("width");
	result.height = get("height");
	endRead();
	return result;
}

Object *LuaTable::toObject() const
{
	beginRead();
	Object *result = get("__object");
	endRead();
	return result;
}

void LuaTable::beginIterator() const
{
	AX_ASSURE(!m_isIteratoring && !m_isReading);

	m_stackTop = lua_gettop(L);
	lua_pushnil(L);

	m_isIteratoring = true;
}

bool LuaTable::nextIterator(Variant &k, Variant &v) const
{
	AX_ASSURE(m_isIteratoring);

	// pop value if need, but need key for iterator
	lua_settop(L,m_stackTop+1);

	k.clear();
	v.clear();

	bool result = lua_next(L,m_index) != 0;

	if (!result) return false;

	k = xReadStack(L,-2);
	v = xReadStack(L,-1);
	return true;
}

void LuaTable::endIterator() const
{
	AX_ASSURE(m_isIteratoring);

	lua_settop(L,m_stackTop);
}


LuaTable & LuaTable::operator=( const LuaTable &rhs )
{
	const_cast<int&>(m_index) = rhs.m_index;
	return *this;
}

AX_END_NAMESPACE
