#include "script_p.h"

AX_BEGIN_NAMESPACE

#if 0
bool MetaInfo::getProperty( Object *obj, const char *propname, const Ref &ret )
{
	Member *member = findMember(propname);

	if (!member)
		return false;

	if (member->getType() != Member::kPropertyType)
		return false;

	Variant::TypeId needReturnType = member->getPropType();
	Variant realRet(ret.getTypeId(), ret.getPointer(), Variant::InitRef);
	bool castRet = false;

	if (ret.getTypeId() != needReturnType) {
		if (needReturnType != Variant::kVoid && ret.getTypeId() != Variant::kVoid) {
			if (!Variant::canCast(needReturnType, ret.getTypeId())) {
				return false;
			} else {
				AX_INIT_STACK_VARIANT(realRet, needReturnType);
				castRet = true;
			}
		}
	}

	int numResult = member->getProperty(obj, realRet.getPointer());
	if (numResult && castRet) {
		return realRet.castTo(ret);
	}

	return true;
}

bool MetaInfo::setProperty( Object *obj, const char *propname, const ConstRef &arg )
{
	Member *member = findMember(propname);

	if (!member)
		return false;

	if (member->getType() != Member::kPropertyType)
		return false;

	Variant::TypeId propTypeId = member->getPropType();
	Variant argValue;
	const void *argData = arg.getPointer();

	if (arg.getTypeId() != propTypeId) {
		if (!Variant::canCast(arg.getTypeId(), propTypeId))
			return false;

		AX_INIT_STACK_VARIANT(argValue, propTypeId);
		bool casted = arg.castTo(argValue);
		if (!casted)
			return false;

		argData = argValue.getPointer();
	}

	return member->setProperty(obj, argData);
}

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
	int kind = Variant::kVoid;

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
			kind = Variant::kColor3;
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
	int kind = Variant::kVoid;

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
		result = (Vector3(x,y,z));
	} else if (hx && hy && hwidth && hheight) {
		kind = Variant::kRect;
		result = (Rect(x,y,width,height));
	} else if (hr && hg && hb) {
		kind = Variant::kColor3;
		result = (Color3(r,g,b));
	} else if (hx && hy) {
		kind = Variant::kPoint;
		result = (Point(x,y));
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
			int val = enumItem.get(1);
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
			AX_RELEASE_ASSERT(kindFromName == Variant::kVoid || kindFromName == Variant::kBool);
			m_propKind = Variant::kBool;
			m_defaultValue = (lua_toboolean(L,-1) ? true : false);
			break;
		case LUA_TNUMBER:
			if (kindFromName == Variant::kVoid || kindFromName == Variant::kFloat) {
				m_propKind = Variant::kFloat;
			} else if (kindFromName == Variant::kInt) {
				m_propKind = Variant::kInt;
			} else {
				Errorf("Why here?");
			}
			m_defaultValue = float(lua_tonumber(L,-1));
			break;
		case LUA_TSTRING:
			if (kindFromName == Variant::kVoid) {
				m_propKind = Variant::kString;
			} else if (isStringType(kindFromName)) {
				m_propKind = kindFromName;
			} else {
				Errorf("Why here?");
			}
			m_defaultValue = (lua_tostring(L,-1));
			break;
		case LUA_TTABLE:
			if (kindFromName == kEnum || kindFromName == kFlag) {
				m_propKind = kindFromName;
				initEnumItems();
			} else {
				m_propKind = checkTableKind(m_defaultValue);
				if (kindFromName != Variant::kVoid && m_propKind != kindFromName) {
					Errorf("Why here?");
				}
				if (!m_group && kindFromName == Variant::kVoid) {
					m_propKind = kGroup;
				}
			}
			break;
		default:
			AX_WRONGPLACE;
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
		if (m_propKind == Variant::kColor3) {
			result = (Color3)result;
		} else if (m_propKind == Variant::kVector3) {
			result = (Vector3)result;
		} else if (m_propKind == Variant::kPoint) {
			result = (Point)result;
		} else if (m_propKind == Variant::kRect) {
			result = (Rect)result;
		} else if (result.getTypeId() == Variant::kTable) {
			result.clear();
		}
	}
	lua_pop(L,1);
	return result;
}
#endif

static std::string getsortkey(const SqProperty *prop)
{
	if (prop->getGroup())
		return prop->getGroup()->getRealName().toString() + prop->getRealName().toString();
	
	if (prop->getPropKind() == Member::kGroup)
		return prop->getRealName().toString();

	return std::string(" ") + prop->getRealName().toString();
}

static bool sqlesser(const SqProperty *a, const SqProperty *b)
{
	return getsortkey(a) < getsortkey(b);
}

SqProperty::SqProperty(const sqObject &key, const sqObject &val, const sqObject &attr)
	: Member(0, Member::kPropertyType)
	, m_group(0)
{
	m_realName = key.toString();
	m_name = m_realName.c_str();

	SQObjectType valType = val.getType();
	val.toVariant(m_default);

	m_propType = m_default.getTypeId();
	m_propKind = m_propType;
	AX_ASSERT(m_propKind != Variant::kVoid);

	// enumitems
	sqObject enums = attr.getValue("enums");
	if (enums.isArray()) {
		int len = enums.len();
		for (int i = 0; i < len; i++) {
			int v = enums.getInt(i);
			std::string k = StringUtil::format("%d", v);
			m_enumItems.push_back(std::make_pair(k,v));
		}
	} else if (enums.isTable()) {
		sqObject ekey, eval;
		enums.beginIteration();
		while (enums.next(ekey, eval)) {
			m_enumItems.push_back(std::make_pair(ekey.toString(), eval.toInteger()));
		}
		enums.endIteration();
	}

	if (m_enumItems.size() && m_propType == Variant::kInt) {
		m_propKind = kEnum;
		return;
	}

	int defineKind = attr.getInt("kind");
	if (!defineKind) return;
	Variant::TypeId defineType = kindToType(defineKind);

	if (defineType != m_propType) return;
	m_propKind = defineKind;
}

SqProperty::SqProperty(const FixedString &name, Kind kind)
	: Member(name, Member::kPropertyType)
	, m_group(0)
{
	m_realName = name;
	m_propKind = kind;
}

bool SqProperty::getProperty(const Object *obj, Variant &ret)
{
	if (!obj) return false;
	if (obj->m_scriptInstance.isNull()) return false;
	obj->m_scriptInstance.getSqObject().getValue(m_realName.c_str()).toVariant(ret);
	return true;
}

bool SqProperty::setProperty(Object *obj, const ConstRef &arg)
{
	if (!obj) return false;
	if (obj->m_scriptInstance.isNull()) return false;

	obj->m_scriptInstance.getSqObject().setValue(m_name.c_str(), arg);
	return true;
}

ScriptClass::ScriptClass(const FixedString &name)
	: m_cppClass(0)
{
	m_name = name;
	HSQUIRRELVM vm = VM;

	sqObject so = g_mainVM->getScoped(name.c_str());
	m_sqObject.getSqObject() = so;

	if (!so.isClass())
		Errorf("wrong type. can't register class");

	so.setValue("_get", sqVM::ms_getClosure);
	so.setValue("_set", sqVM::ms_setClosure);
	so.setTypeTag(&__Object_c_decl);

	sqObject key, val, attr;

	// get cppname
	attr = so.getAttributes();
	const char *cppClass = attr.getString("cppClass");
	while (!cppClass) {
		sqObject base = so.getBase();
		if (!base.isClass())
			Errorf("Base isn't a class");

		cppClass = base.getAttributes().getString("cppClass");
	}
	m_cppName = cppClass;

	so.beginIteration();

	while (so.next(key, val)) {
		if (so.isClosure() || so.isNativeClosure())
			continue;

		attr = so.getAttributes(key.toString());

		addProperty(key, val, attr);
	}

	so.endIteration();

	std::sort(m_properties.begin(), m_properties.end(), sqlesser);
}

void ScriptClass::addProperty(const sqObject &key, const sqObject &val, const sqObject &attr)
{
	if (!attr.getBool("editable"))
		return;

	SqProperty *prop = new SqProperty(key, val, attr);

	m_properties.push_back(prop);
	m_propDict[prop->getName()] = prop;

	const char *groupName = attr.getString("group");

	if (!groupName) return;

	SqProperty *group = 0;
	SqPropertyDict::iterator it = m_propDict.find(groupName);

	if (it == m_propDict.end()) {
		group = new SqProperty(groupName, Member::kGroup);
		m_propDict[groupName] = group;
		m_properties.push_back(group);
	} else {
		group = it->second;
	}

	prop->m_group = group;
}

AX_END_NAMESPACE

