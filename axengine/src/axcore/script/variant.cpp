#include "script_p.h"
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>

AX_BEGIN_NAMESPACE

typedef boost::mpl::vector<void,bool,int,float,String,Object *, LuaTable,Vector3,Color3,Point,Rect,Matrix3x4> VariantTL;


typedef bool (*castFunc)(const void *from, void *to);

template <class T, class Q, class Can = boost::mpl::int_<1> >
class CastHelper_ {
	static bool CastFunc(const void *from, void *to)
	{
		if (std::tr1::is_convertible<T,Q>::value) {
			*(Q *)to = *(const T *)from;
			return true;
		}
		return false;
	}
};

template <class T, class Q, boost::mpl::int_<2> >
class CastHelper_ {
	static bool CastFunc(const void *from, void *to)
	{
		return false;
	}
};


struct TypeHandler {
	int dataSize;
	bool canCast[Variant::kMaxType];
	castFunc castFuncs[Variant::kMaxType];

	virtual void construct(void *ptr) = 0;
	virtual void destruct(void *ptr) = 0;
};

#define CANCAST(x) canCast[x] = std::tr1::is_convertible<T, boost::mpl::at<VariantTL, boost::mpl::int_<x> >::type>::value
#define CASTFUNC(x) boost::mpl::if_< \
	std::tr1::is_convertible<T, boost::mpl::at<VariantTL, boost::mpl::int_<x> >::type>::value, \
	CastFunc<T, boost::mpl::at<VariantTL, boost::mpl::int_<x> >::type>, int \
>::type

template <class T>
struct TypeHandler_ : public TypeHandler
{
	TypeHandler_()
	{
		dataSize = sizeof(T);

//		CANCAST(0);
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

		boost::mpl::if_<
			boost::mpl::int_<std::tr1::is_convertible<T, boost::mpl::at<VariantTL, boost::mpl::int_<0> >::type>::value>,
			1, 2
		>::type t;
#if 0
//		CASTFUNC(0);
//		castFuncs[1] = CastFunc<T, boost::mpl::at<VariantTL, boost::mpl::int_<1> >::type>;
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
#endif
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

void testhandler()
{
	TypeHandler *th = new TypeHandler_<int>();
}

String Variant::toString() const
{
	String result;
	switch (type) {
	case kEmpty:
		break;
	case kBool:
		StringUtil::sprintf(result, "%d", boolval);
		break;
	case kInt:
		StringUtil::sprintf(result, "%d", intval);
		break;
	case kFloat:
		StringUtil::sprintf(result, "%f", realval);
		break;
	case kString:
		result = *str;
		break;
	case kObject:
		break;
	case kTable:
		break;
	case kVector3:
		{
			Vector3 v = *this;
			return v.toString();
			break;
		}
	case kColor3:
		{
			Color3 v = *this;
			return v.toString();
			break;
		}
	case kPoint:
		{
			Point v = *this;
			return v.toString();
			break;
		}
	case kRect:
		{
			Rect v = *this;
			return v.toString();
			break;
		}
	case kMatrix3x4:
		{
			return mtr->toString();
		}
	default:
		AX_NO_DEFAULT;
	}
	return result;
}

void Variant::fromString(Type t, const char *str) {
	clear();

	switch (t) {
	case kEmpty:
		break;
	case kBool:
		this->set(atoi(str) ? true : false);
		break;
	case kInt:
		this->set(atoi(str));
		break;
	case kFloat:
		this->set(atof(str));
		break;
	case kString:
		this->set(str);
		break;
	case kObject:
		break;
	case kTable:
		break;
	case kVector3:
		{
			Vector3 v;
			v.fromString(str);
			this->set(v);
			break;
		}
	case kColor3:
		{
			Color3 v;
			v.fromString(str);
			this->set(v);
			break;
		}
	case kPoint:
		{
			Point v;
			v.fromString(str);
			this->set(v);
			break;
		}
	case kRect:
		{
			Rect v; v.fromString(str); this->set(v);
			break;
		}
	case kMatrix3x4:
		{
			Matrix3x4 v;
			v.fromString(str);
			this->set(v);
			break;
		}
	default:
		AX_NO_DEFAULT;
	}
}

int Variant::getTypeSize(Type t)
{
	switch (t) {
	case kEmpty: return 0;
	case kBool: return sizeof(bool);
	case kInt: return sizeof(int);
	case kFloat: return sizeof(float);
	case kString: return sizeof(String);
	case kObject: return sizeof(Object *);
	case kTable: return sizeof(LuaTable);
	case kVector3: return sizeof(Vector3);
	case kColor3: return sizeof(Color3);
	case kPoint: return sizeof(Point);
	case kRect: return sizeof(Rect);
	case kMatrix3x4: return sizeof(Matrix3x4);
	default: AX_NO_DEFAULT;
	}
	return 0;
}

bool Variant::canCast(Type fromType, Type toType)
{
	if (fromType == toType)
		return true;

	switch (fromType) {
	case kEmpty: return false;
	case kBool:
		switch (toType) {
		case kInt:
		case kFloat:
			return true;
		}
		return false;
	case kInt:
		switch (toType) {
		case kBool:
		case kFloat:
			return true;
		}
		return false;
	case kFloat:
		switch (toType) {
		case kBool:
		case kInt:
			return true;
		}
		return false;
	case kString:
	case kObject:
	case kTable:
	case kVector3:
	case kColor3:
	case kPoint:
	case kRect:
	case kMatrix3x4:
		return false;
	default: AX_NO_DEFAULT;
	}
	return false;
}

bool Variant::rawCast(Type fromType, const void *fromData, Type toType, void *toData)
{
	if (!canCast(fromType, toType))
		return false;

	switch (fromType) {
	case kEmpty: return false;
	case kBool:
		switch (toType) {
		case kInt:
			{
				const bool &from = *(const bool *)fromData;
				int &to = *(int *)toData;
				to = from;
				return true;
			}
		case kFloat:
			{
				const bool &from = *(const bool *)fromData;
				float &to = *(float *)toData;
				to = from;
				return true;
			}
		}
		return false;
	case kInt:
		switch (toType) {
		case kBool:
			{
				const int &from = *(const int *)fromData;
				bool &to = *(bool *)toData;
				to = from != 0;
				return true;
			}
		case kFloat:
			{
				const int &from = *(const int *)fromData;
				float &to = *(float *)toData;
				to = from;
				return true;
			}
		}
		return false;
	case kFloat:
		switch (toType) {
		case kBool:
			{
				const float &from = *(const float *)fromData;
				bool &to = *(bool *)toData;
				to = from != 0.0f;
				return true;
			}
		case kInt:
			{
				const float &from = *(const float *)fromData;
				int &to = *(int *)toData;
				to = from;
				return true;
			}
		}
		return false;
	case kString:
	case kObject:
	case kTable:
	case kVector3:
	case kColor3:
	case kPoint:
	case kRect:
	case kMatrix3x4:
		return false;
	default: AX_NO_DEFAULT;
	}
	return false;
}


//--------------------------------------------------------------------------
// class LuaTable
//--------------------------------------------------------------------------

LuaTable::LuaTable(int index)
	: m_index(index)
	, m_isReading(false)
	, m_isIteratoring(false)
	, m_stackTop(0)
{
	if (m_index < 0) {
		m_index = lua_gettop(L) + m_index + 1;
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

	if (result.type != Variant::kTable) {
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
	result.r = get("r").toFloat();
	result.g = get("g").toFloat();
	result.b = get("b").toFloat();
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

AX_END_NAMESPACE
