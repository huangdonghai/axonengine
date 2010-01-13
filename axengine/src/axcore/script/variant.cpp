#include "script_p.h"

AX_BEGIN_NAMESPACE

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

Rgb LuaTable::toColor() const
{
	Rgb result;
	beginRead();
	result.r = get("r").toFloat() * 255;
	result.g = get("g").toFloat() * 255;
	result.b = get("b").toFloat() * 255;
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
