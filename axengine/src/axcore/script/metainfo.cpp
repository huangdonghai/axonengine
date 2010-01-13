#include "script_p.h"

AX_BEGIN_NAMESPACE

bool lesser(const ScriptProp *a, const ScriptProp *b)
{
	if (!a->grouped() && b->grouped()) {
		return true;
	}

	if (a->grouped() && !b->grouped()) {
		return false;
	}
	return a->m_realName < b->m_realName;
}

void ClassInfo::initScriptProps()
{
	// init spawn arguments
	int top = lua_gettop(L);

	xGetGlobalScoped(L, m_className.c_str());		// 1
	AX_ASSERT(lua_istable(L,-1));

	lua_getfield(L, -1, "Properties");
	if (!lua_istable(L,-1)) {
		lua_settop(L, top);
		return;
	}

	// iterator spawn args

	/* table is in index -2 */
	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0) {
		/* 'key' at -2 and 'value' at -1 */
		//			Printf("%s - %s\n", lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)));

		AX_ASSERT(lua_isstring(L,-2));
		ScriptProp *sa = new ScriptProp();

		sa->m_realName = lua_tostring(L, -2);
		sa->m_group = 0;
		sa->init();
		m_scriptProps[sa->m_showName] = sa;
		m_scriptPropSeq.push_back(sa);

		if (sa->m_propKind == ScriptProp::kGroup) {
			lua_pushnil(L);
			while (lua_next(L,-2)) {
				AX_ASSERT(lua_isstring(L,-2));
				ScriptProp *child = new ScriptProp();

				child->m_group = sa;
				child->m_realName = lua_tostring(L, -2);
				child->init();

				if (m_scriptProps.find(child->m_showName) != m_scriptProps.end()) {
					Errorf("duplicated script property name '%s'", child->m_showName.c_str());
				}
				m_scriptProps[child->m_showName] = child;
				m_scriptPropSeq.push_back(child);
				lua_pop(L, 1);
			}
		}

		/* remove 'value' £»reserver 'key' for next iterator */
		lua_pop(L, 1);
	}
	lua_pop(L,1);	// pop SpawnArgs
	lua_pop(L,1);	// pop class

	AX_ASSERT(lua_gettop(L) == top);

	std::sort(m_scriptPropSeq.begin(), m_scriptPropSeq.end(), lesser);
}

Variant ClassInfo::getField(const String &field) const
{
	Variant result;

	if (xGetGlobalScoped(L, (m_className + "." + field).c_str())) {
		result = xReadStack(L, -1);
	}

	lua_pop(L, 1);

	return result;
}

inline int ScriptProp::checkNameKind()
{
	int kind = Variant::kEmpty;

	size_t pos = m_realName.find('_');
	if (pos != String::npos) {
		String ks(m_realName.substr(0,pos));
		m_showName = m_realName.substr(pos+1, m_realName.size()-pos-1);

		if (ks == "bool") {
			kind = Variant::kBool;
		} else if (ks == "int") {
			kind = Variant::kInt;
		} else if (ks == "float") {
			kind = Variant::kFloat;
		} else if (ks == "str") {
			kind = Variant::kString;
		} else if (ks == "mdl") {
			kind = ScriptProp::kModel;
		} else if (ks == "tex") {
			kind = ScriptProp::kTexture;
		} else if (ks == "snd") {
			kind = ScriptProp::kSound;
		} else if (ks == "mtr") {
			kind = ScriptProp::kMaterial;
		} else if (ks == "anim") {
			kind = ScriptProp::kAnimation;
		} else if (ks == "spt") {
			kind = ScriptProp::kSpeedTree;
		} else if (ks == "enum") {
			kind = ScriptProp::kEnum;
		} else if (ks == "flag") {
			kind = ScriptProp::kFlag;
		} else if (ks == "vec") {
			kind = Variant::kVector3;
		} else if (ks == "color") {
			kind = Variant::kColor;
		} else if (ks == "point") {
			kind = Variant::kPoint;
		} else if (ks == "rect") {
			kind = Variant::kRect;
		} else {
			Debugf("Error, don't know SpawnArg kind '%s'", ks.c_str());
		}
	} else {
		m_showName = m_realName;
	}

	if (m_group) {
		m_realName = m_group->m_realName + '.' + m_realName;
	}

	return kind;
}

inline bool ScriptProp::isStringType(int kind)
{
	return kindToType(kind) == Variant::kString;
}


#define CHECK_FILED(x) \
	lua_getfield(L, -1, #x); \
	if (h##x = lua_isnumber(L, -1) ? true : false) { \
	x = lua_tonumber(L, -1);	\
	} \
	lua_pop(L, 1) 


int ScriptProp::checkTableKind(Variant &result)
{
	int kind = Variant::kEmpty;

	bool hx, hy, hz, hr, hg, hb, hwidth, hheight;
	float x, y, z, r, g, b, width, height;

	CHECK_FILED(x);
	CHECK_FILED(y);
	CHECK_FILED(z);
	CHECK_FILED(r);
	CHECK_FILED(g);
	CHECK_FILED(b);
	CHECK_FILED(width);
	CHECK_FILED(height);

	if (hx && hy && hz) {
		kind = Variant::kVector3;
		result.set(Vector3(x,y,z));
	} else if (hx && hy && hwidth && hheight) {
		kind = Variant::kRect;
		result.set(Rect(x,y,width,height));
	} else if (hr && hg && hb) {
		kind = Variant::kColor;
		result.set(Rgb(r,g,b));
	} else if (hx && hy) {
		kind = Variant::kPoint;
		result.set(Point(x,y));
	}

	return kind;
}

void ScriptProp::initEnumItems()
{
	LuaTable enumTable(-1);
	enumTable.beginRead();
	for (int i = 0; i < enumTable.getLength(); i++) {
		LuaTable enumItem = enumTable.get(i);
		enumItem.beginRead();
		if (enumItem.getLength() >= 2) {
			String key = enumItem.get(0).toString();
			int val = enumItem.get(1).toInt();
			m_enumItems.push_back(std::make_pair(key, val));
		}
		enumItem.endRead();
	}
	enumTable.endRead();
#if 1
	for (lua_pushnil(L); lua_next(L,-2); lua_pop(L, 1)) {
		if (lua_type(L,-2) != LUA_TSTRING)
			continue;

		const char *name = lua_tostring(L, -2);

		if (name[0] == '_') {
			continue;
		}

		if (lua_isnumber(L,-1)) {
			m_enumItems.push_back(std::make_pair<String,int>(name, lua_tonumber(L,-1)));
		}
	}
#endif
}

void ScriptProp::init()
{
	int kindFromName = checkNameKind();

	int valuetype = lua_type(L,-1);
	switch (valuetype) {
		case LUA_TBOOLEAN:
			AX_ASSURE(kindFromName == Variant::kEmpty || kindFromName == Variant::kBool);
			m_propKind = Variant::kBool;
			m_defaultValue.set(lua_toboolean(L,-1) ? true : false);
			break;
		case LUA_TNUMBER:
			if (kindFromName == Variant::kEmpty || kindFromName == Variant::kFloat) {
				m_propKind = Variant::kFloat;
			} else if (kindFromName == Variant::kInt) {
				m_propKind = Variant::kInt;
			} else {
				Errorf("Why here?");
			}
			m_defaultValue.set(lua_tonumber(L,-1));
			break;
		case LUA_TSTRING:
			if (kindFromName == Variant::kEmpty) {
				m_propKind = Variant::kString;
			} else if (isStringType(kindFromName)) {
				m_propKind = kindFromName;
			} else {
				Errorf("Why here?");
			}
			m_defaultValue.set(lua_tostring(L,-1));
			break;
		case LUA_TTABLE:
			if (kindFromName == kEnum || kindFromName == kFlag) {
				m_propKind = kindFromName;
				initEnumItems();
			} else {
				m_propKind = checkTableKind(m_defaultValue);
				if (kindFromName != Variant::kEmpty && m_propKind != kindFromName) {
					Errorf("Why here?");
				}
				if (!m_group && kindFromName == Variant::kEmpty) {
					m_propKind = kGroup;
				}
			}
			break;
		default:
			AX_NO_DEFAULT;
			break;
	}

	m_name = m_showName.c_str();
	m_propType = kindToType(m_propKind);
}

ScriptProp::ScriptProp() : Member(0, Member::kPropertyType)
{}

void ScriptProp::setProperty(Object *obj, const Variant &val)
{
	if (m_propKind == kGroup)
		return;

	int top = lua_gettop(L);

	lua_pushlightuserdata(L, const_cast<Object*>(obj));
	lua_rawget(L, LUA_REGISTRYINDEX);

	xSetScoped(L, ("Properties." + m_realName).c_str(), val);

	lua_settop(L, top);
}

Variant ScriptProp::getProperty(const Object *obj)
{
	Variant result;

	if (m_propKind == kGroup)
		return result;

	lua_pushlightuserdata(L, const_cast<Object*>(obj));
	lua_rawget(L, LUA_REGISTRYINDEX);
	if (xGetScoped(L, ("Properties." + m_realName).c_str())) {
		result = xReadStack(L,-1);
		if (m_propKind == Variant::kColor) {
			result = (Rgb)result;
		} else if (m_propKind == Variant::kVector3) {
			result = (Vector3)result;
		} else if (m_propKind == Variant::kPoint) {
			result = (Point)result;
		} else if (m_propKind == Variant::kRect) {
			result = (Rect)result;
		} else if (result.type == Variant::kTable) {
			result.clear();
		}
	}
	lua_pop(L,1);
	return result;
}

AX_END_NAMESPACE
