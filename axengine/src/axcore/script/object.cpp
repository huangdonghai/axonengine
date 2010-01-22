#include "script_p.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class Object
//--------------------------------------------------------------------------

MetaInfo *Object::m_metaInfo = nullptr;

Object::Object()
{
	m_classInfo = 0;

	int top = lua_gettop(L);
	lua_pushlightuserdata(L, this);
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);		// stack poped

	// get table just created
	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	// set metatable
	luaL_getmetatable(L, OBJECT_MT);
	lua_setmetatable(L, -2);

	// set __object
	lua_pushliteral(L, "__object");
	lua_pushlightuserdata(L, this);
	lua_rawset(L, -3);

	// balance stack
	lua_pop(L, 1);

	// create squirrel object
	sq_pushregistrytable(VM);
	sq_pushuserpointer(VM, this);
	CreateNativeClassInstance(VM, "Object_c", this);
	sq_rawset(VM, -3);
	sq_pop(VM, 1);
}

Object::~Object()
{
#if 1
	invoke_onFinalize();

	resetObjectName();

	lua_pushlightuserdata(L, this);
	lua_pushnil(L);
	lua_rawset(L, LUA_REGISTRYINDEX);
#endif
	//		gScriptSystem->removeObject(this);
}

Member *Object::findMember(const char *name) const
{
	MetaInfo *typeinfo = getMetaInfo();

	while (typeinfo) {
		Member *member = typeinfo->findMember(name);
		if (member) return member;
		typeinfo = typeinfo->getBase();
	}

	if (!m_classInfo) {
		return nullptr;
	}

	ScriptPropDict::const_iterator it = m_classInfo->m_scriptProps.find(name);
	if (it == m_classInfo->m_scriptProps.end()) {
		return nullptr;
	}

	return it->second;
}

MetaInfo *Object::getMetaInfo() const
{
	if (!m_metaInfo) {
		Object::registerMetaInfo();
		AX_ASSERT(m_metaInfo);
	}

	return m_metaInfo;
}

MetaInfo *Object::registerMetaInfo()
{
	if (!m_metaInfo) {
		m_metaInfo = new MetaInfo_<Object>("Object", nullptr);
		m_metaInfo->addProperty("objectName", &Object::get_objectName, &Object::set_objectName);

		g_scriptSystem->registerType(m_metaInfo);
	}
	return m_metaInfo;
}


void Object::set_objectName(const String &name)
{
	setObjectName(name, false);
}

String Object::get_objectName() const
{
	return m_objectName;
}


bool Object::inherits(const char *cls) const
{
	MetaInfo *typeinfo = getMetaInfo();

	for (; typeinfo; typeinfo=typeinfo->getBase()) {
		if (Strequ(cls, typeinfo->getName())) {
			return true;
		}
	}

	return false;
}

Variant Object::getProperty(const char *name) const
{
	Member *m = findMember(name);
	if (!m || !m->isProperty()) {
		return Variant();
	}

	return m->getProperty(this);
}

bool Object::setProperty(const char *name, const Variant &value)
{
	Member *m = findMember(name);
	if (!m || !m->isProperty() || m->isConst()) {
		return false;
	}

	m->setProperty(this, value);

	return true;
}

bool Object::setProperty(const char *name, const char *value)
{
	Member *member = findMember(name);

	if (!member) {
		return false;
	}

	if (!member->isProperty()) {
		return false;
	}

	if (member->isConst()) {
		return false;
	}

	Variant var;
	var.fromString(member->getPropType(), value);
	member->setProperty(this, var);

	return true;
}

void Object::writeProperties(File *f, int indent) const
{
	String indstr(indent*2, ' ');
#define INDENT if (indent) f->printf("%s", indstr.c_str());

	// write properties
	MetaInfo *typeinfo = getMetaInfo();

	while (typeinfo) {
		const MemberSeq &members = typeinfo->getMembers();

		AX_FOREACH(Member *m, members) {
			if (!m->isProperty()) {
				continue;
			}

			if (m->isConst()) {
				continue;
			}

			INDENT; f->printf("  %s=\"%s\"\n", m->getName(), getProperty(m->getName()).toString().c_str());
		}

		typeinfo = typeinfo->getBase();
	}

	// write script properties
	const ClassInfo *classinfo = getClassInfo();
	if (!classinfo) return;

	const ScriptPropSeq &props = classinfo->m_scriptPropSeq;

	AX_FOREACH(ScriptProp *m, props) {
		if (m->getPropKind() == ScriptProp::kGroup)
			continue;

		INDENT; f->printf("  %s=\"%s\"\n", m->getName(), m->getProperty(this).toString().c_str());
	}

#undef INDENT
}

void Object::readProperties(const TiXmlElement *node)
{
	const TiXmlAttribute *attr = node->FirstAttribute();

	for (; attr; attr = attr->Next()) {
		this->setProperty(attr->Name(), attr->Value());
	}
}

void Object::copyPropertiesFrom(const Object *rhs)
{
	// write properties
	MetaInfo *typeinfo = rhs->getMetaInfo();

	while (typeinfo) {
		// don't copy objectname
		if (typeinfo == Object::m_metaInfo)
			break;

		const MemberSeq &members = typeinfo->getMembers();

		AX_FOREACH(Member *m, members) {
			if (!m->isProperty()) {
				continue;
			}

			if (m->isConst()) {
				continue;
			}

			setProperty(m->getName(), m->getProperty(rhs));
		}

		typeinfo = typeinfo->getBase();
	}

	// write script properties
	const ClassInfo *classinfo = getClassInfo();
	if (!classinfo) return;

	const ScriptPropSeq &props = classinfo->m_scriptPropSeq;

	AX_FOREACH(ScriptProp *m, props) {
		if (m->getPropKind() == ScriptProp::kGroup)
			continue;

		setProperty(m->getName(), m->getProperty(rhs));
	}
}


void Object::initClassInfo(const ClassInfo *ci)
{
	int top = lua_gettop(L);

	lua_pushlightuserdata(L, this);

	lua_pushliteral(L, "AX_CREATE_OBJECT");
	lua_rawget(L, LUA_GLOBALSINDEX);
	AX_ASSERT(lua_isfunction(L, -1));
	if (!xGetGlobalScoped(L, ci->m_className.c_str())) {
		Errorf("can't find script class '%s'", ci->m_className.c_str());
	}
	AX_ASSERT(lua_istable(L, -1));
	int status = lua_pcall(L, 1, 1, 0);
	if (status != 0) {
		xReport(L, status, 0);
		lua_settop(L,top);
		return;
	}
	AX_ASSERT(lua_istable(L, -1));

	lua_rawset(L, LUA_REGISTRYINDEX);		// stack poped

	// get table just created
	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	// set metatable
	luaL_getmetatable(L, OBJECT_MT);
	lua_setmetatable(L, -2);

	// set __object
	lua_pushliteral(L, "__object");
	lua_pushlightuserdata(L, this);
	lua_rawset(L, -3);

	// balance stack
	lua_pop(L, 1);

	AX_ASSERT(lua_gettop(L) == 0);

	m_classInfo = ci;

	// TODO: rebind to object name
}

void Object::setObjectName(const String &name, bool force)
{
	if (m_objectName == name && !force) {
		return;
	}

	resetObjectName();

	m_objectName = name;
	{
		int top = sq_gettop(VM);
		sq_pushroottable(VM);
		sq_pushregistrytable(VM);
		sq_pushstring(VM, name.c_str(), name.size());
		sq_pushuserpointer(VM, this);
		sq_rawget(VM, -3);
		sq_rawset(VM, -4);
		sq_settop(VM, top);
	}
	m_objectNamespace = getNamespace();

	if (m_objectName.empty())
		return;

	int top = lua_gettop(L);

	bool v = xGetGlobalScoped(L, m_objectNamespace.c_str());
	if (!v) {
		Errorf("Invalid object namespace");
	}
	int ns = lua_gettop(L);

	if (!lua_istable(L, ns)) {
		Errorf("Object::setObjectName: can't find object namespace '%s'", getNamespace().c_str());
	}

	// get from registry first
	lua_getfield(L, ns, m_objectName.c_str());
	if (!lua_isnil(L, -1)) {
		Errorf("Object::set_objectName: object already exist");
	}
	lua_pop(L, 1);

	// set new name
	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	AX_ASSERT(lua_istable(L, -1));

	lua_setfield(L, ns, m_objectName.c_str());

	// balance stack
	lua_settop(L, top);
}

void Object::resetObjectName()
{
	if (m_objectName.empty())
		return;

	int top = lua_gettop(L);

	bool v = xGetGlobalScoped(L, m_objectNamespace.c_str());
	if (!v) {
		Errorf("Invalid object namespace");
	}
	int ns = lua_gettop(L);

	if (!lua_istable(L, ns)) {
		Errorf("Object::setObjectName: can't find object namespace '%s'", getNamespace().c_str());
	}

	if (!m_objectName.empty()) {
		// free old link
		lua_pushnil(L);
		lua_setfield(L, ns, m_objectName.c_str());

		// FIXME: check
		lua_getfield(L, ns, m_objectName.c_str());
		if (!lua_isnil(L, -1)) {
			Errorf("Object::set_objectName: object already exist");
		}
		lua_pop(L, 1);
	}

	lua_settop(L, top);
}

void Object::doPropertyChanged()
{
	onPropertyChanged();
}

void Object::onPropertyChanged()
{
	invoke_onPropertyChanged();
}


void Object::invoke_onInit()
{
	invokeCallback("onInit");
}

void Object::invoke_onFinalize()
{
	invokeCallback("onFinalize");
}

void Object::invoke_onPropertyChanged()
{
	invokeCallback("onPropertyChanged");
}

void Object::invokeCallback(const String &callback)
{
	int top = lua_gettop(L);

	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	xPushString(L, callback);
	lua_gettable(L, -2);

	if (!lua_isfunction(L, -1)) {
		lua_settop(L, top);
		return;
	}

	// push self
	lua_pushvalue(L, -2);

	xPcall(L, 1, 0);

	lua_settop(L, top);
	return;
}

void Object::invokeCallback(const String &callback, const Variant &param)
{
	int top = lua_gettop(L);

	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	xPushString(L, callback);
	lua_gettable(L, -2);

	if (!lua_isfunction(L, -1)) {
		lua_settop(L, top);
		return;
	}

	// push self
	lua_pushvalue(L, -2);

	// push param
	xPushStack(L, param);

	xPcall(L, 2, 0);

	lua_settop(L, top);
	return;
}

void Object::invokeMethod(const char *name, const Variant &arg1)
{
	Member *member = findMember(name);

	if (!member || !member->isMethod()) {
		return;
	}

	VariantSeq vs;
	vs.push_back(arg1);
	member->invoke(this, vs);
}

void Object::invokeMethod(const char *name, const Variant &arg1, const Variant &arg2)
{
	Member *member = findMember(name);

	if (!member || !member->isMethod()) {
		return;
	}

	VariantSeq vs;
	vs.push_back(arg1);
	vs.push_back(arg2);
	member->invoke(this, vs);
}

bool Object::isClass(const char *classname) const
{
	if (!m_classInfo) {
		return false;
	}

	return m_classInfo->m_className == classname;
}

void Object::setRuntime(const char *name, const Variant &val)
{
	int top = lua_gettop(L);

	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	xPushStack(L, val);

	lua_setfield(L, -2, name);

	lua_settop(L, top);

	return;
}

Variant Object::getRuntime(const char *name)
{
	int top = lua_gettop(L);

	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	lua_getfield(L, -2, name);
	Variant result = xReadStack(L,-1);

	if (result.getTypeId() == Variant::kTable) {
		Errorf("cann't get Lua table in getRuntime");
	}

	lua_settop(L, top);

	return result;
}

AX_END_NAMESPACE
