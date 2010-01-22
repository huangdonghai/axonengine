#include "script_p.h"

AX_BEGIN_NAMESPACE

//--------------------------------------------------------------------------
// class Object
//--------------------------------------------------------------------------

Object::Object()
{
#if 0
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
#endif
}

Object::~Object()
{
#if 0
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

#if 0
	if (!m_classInfo) {
		return nullptr;
	}

	ScriptPropDict::const_iterator it = m_classInfo->m_scriptProps.find(name);
	if (it == m_classInfo->m_scriptProps.end()) {
		return nullptr;
	}
	return it->second;
#else
	if (!m_sqClass)
		return 0;

	return m_sqClass->findMember(name);
#endif
}

MetaInfo *Object::getMetaInfo() const
{
	return Object::registerMetaInfo();
}

MetaInfo *Object::registerMetaInfo()
{
	static MetaInfo *ms_metaInfo = 0;

	if (!ms_metaInfo) {
		ms_metaInfo = new MetaInfo_<Object>("Object", nullptr);
		ms_metaInfo->addProperty("objectName", &Object::get_objectName, &Object::set_objectName);

		g_scriptSystem->registerType(ms_metaInfo);
	}
	return ms_metaInfo;
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

bool Object::getProperty(const char *name, Variant &ret) const
{
	Member *m = findMember(name);
	if (!m || !m->isProperty()) {
		return false;
	}

	return m->getProperty(this, ret);
}

bool Object::setProperty(const char *name, const Variant &value)
{
	Member *m = findMember(name);
	if (!m || !m->isProperty() || m->isConst()) {
		return false;
	}

	m->setProperty(this, ConstRef(value.getTypeId(), value.getPointer()));

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
	member->setProperty(this, ConstRef(var.getTypeId(), var.getPointer()));

	return true;
}

void Object::writeProperties(File *f, int indent) const
{
	String indstr(indent*2, ' ');
#define INDENT if (indent) f->printf("%s", indstr.c_str());

	// write properties
	MetaInfo *typeinfo = getMetaInfo();

	Variant prop;
	while (typeinfo) {
		const MemberSeq &members = typeinfo->getMembers();

		AX_FOREACH(Member *m, members) {
			if (!m->isProperty()) continue;

			if (m->isConst()) continue;

			if (!m->getProperty(this, prop)) continue;

			INDENT; f->printf("  %s=\"%s\"\n", m->getName(), prop.toString().c_str());
		}

		typeinfo = typeinfo->getBase();
	}

	// write script properties
	const SqClass *classinfo = getScriptClass();
	if (!classinfo) return;

	const SqProperties &props = classinfo->getMembers();

	AX_FOREACH(SqProperty *m, props) {
		if (m->getPropKind() == Member::kGroup)
			continue;

		if (!m->getProperty(this, prop)) continue;

		INDENT; f->printf("  %s=\"%s\"\n", m->getName(), prop.toString().c_str());
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
	Variant prop;

	while (typeinfo) {
		// don't copy objectname
		if (typeinfo == Object::registerMetaInfo())
			break;

		const MemberSeq &members = typeinfo->getMembers();

		AX_FOREACH(Member *m, members) {
			if (!m->isProperty()) {
				continue;
			}

			if (m->isConst()) {
				continue;
			}

			if (!m->getProperty(rhs, prop)) continue;

			setProperty(m->getName(), prop);
		}

		typeinfo = typeinfo->getBase();
	}

	// write script properties
	const SqClass *classinfo = getScriptClass();
	if (!classinfo) return;

	const SqProperties &props = classinfo->getMembers();

	AX_FOREACH(SqProperty *m, props) {
		if (m->getPropKind() == Member::kGroup)
			continue;

		if (!m->getProperty(rhs, prop)) continue;

		setProperty(m->getName(), prop);
	}
}

#if 0
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
#endif

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

#if 0
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
#endif
}

void Object::resetObjectName()
{
	if (m_objectName.empty())
		return;

#if 0
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
#endif
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
#if 0
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
#endif
}

void Object::invokeCallback(const String &callback, const Variant &param)
{
#if 0
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
#endif
}

#if 0
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
#endif
bool Object::isClass(const char *classname) const
{
#if 0
	if (!m_classInfo) {
		return false;
	}

	return m_classInfo->m_className == classname;
#else
	return false;
#endif
}

void Object::setRuntime(const char *name, const Variant &val)
{
#if 0
	int top = lua_gettop(L);

	lua_pushlightuserdata(L, this);
	lua_rawget(L, LUA_REGISTRYINDEX);

	xPushStack(L, val);

	lua_setfield(L, -2, name);

	lua_settop(L, top);

	return;
#endif
}

Variant Object::getRuntime(const char *name)
{
#if 0
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
#else
	return Variant();
#endif
}

void Object::initScriptClass(const SqClass *sqclass)
{

}

bool Object::invokeMethod(const char *methodName, const Ref &ret,
							const ConstRef &arg0, const ConstRef &arg1, const ConstRef &arg2,
							const ConstRef &arg3, const ConstRef &arg4)
{
	MetaInfo *mi = getMetaInfo();
	if (!mi) return false;

	Member *member = mi->findMember(methodName);

	if (!member)
		return false;

	if (member->getType() != Member::kMethodType)
		return false;

	// check types
	Variant::TypeId needReturnType = member->getReturnType();
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

	ConstRef args[] = { arg0, arg1, arg2, arg3, arg4 };

	Variant realArg[Member::MaxArgs];
	const Variant::TypeId *needTypeIds = member->getArgsType();
	for (int i = 0; i <member->argc(); i++) {
		if (args[i].getTypeId() == needTypeIds[i])
			continue;

		if (!Variant::canCast(args[i].getTypeId(), needTypeIds[i]))
			return false;

		AX_INIT_STACK_VARIANT(realArg[i], needTypeIds[i]);

		bool casted = args[i].castTo(realArg[i]);
		if (!casted)
			return false;

		args[i].set(realArg[i].getTypeId(), realArg[i].getPointer());
	}

	const void *argDatas[] = {
		args[0].getPointer(), args[1].getPointer(), args[2].getPointer(), args[3].getPointer(), args[4].getPointer()
	};

	// really call
	int numResult = member->invoke(this, realRet.getPointer(), argDatas);

	if (numResult && castRet) {
		return realRet.castTo(ret);
	}

	return true;
}



AX_END_NAMESPACE
