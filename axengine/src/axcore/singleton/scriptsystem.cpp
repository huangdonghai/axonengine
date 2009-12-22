/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

#define OBJECT_MT "__object_mt"
#define OBJECT_NAME "__object"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>

	using namespace Axon;

	static lua_State* L;

	static void l_message(const char *pname, const char *msg) {
		if (pname) Axon::Debugf("%s: ", pname);
		Axon::Errorf("%s\n", msg);
	}


	static int xReport(lua_State *L, int status, const char* progname) {
		if (status && !lua_isnil(L, -1)) {
			const char *msg = lua_tostring(L, -1);
			if (msg == NULL) msg = "(error object is not a string)";
			l_message(progname, msg);
			lua_pop(L, 1);
		}
		return status;
	}


	static int luaB_print (lua_State *L) {
		int n = lua_gettop(L);  /* number of arguments */
		int i;
		lua_getglobal(L, "tostring");
		for (i=1; i<=n; i++) {
			const char *s;
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */
			lua_call(L, 1, 1);
			s = lua_tostring(L, -1);  /* get result */
			if (s == NULL)
				return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
			if (i>1) Axon::Printf("\t");
			Axon::Printf("%s",s);
			lua_pop(L, 1);  /* pop result */
		}
		Axon::Printf("\n");
		return 0;
	}

	static int luaB_dofile (lua_State *L) {
		const char *fname = luaL_optstring(L, 1, NULL);
		int n = lua_gettop(L);
#if 0
		if (luaL_loadfile(L, fname) != 0) lua_error(L);
#else
		size_t filesize;
		char* filebuf;

		filesize = Axon::g_fileSystem->readFile(Axon::g_scriptSystem->getPackagePath() + fname, (void**)&filebuf);

		if (!filesize || !filebuf)
			return 0;

		int s = luaL_loadbuffer(L, filebuf, filesize, fname);
		if (s) {
			lua_error(L);
		}

#endif
		lua_call(L, 0, LUA_MULTRET);
		return lua_gettop(L) - n;
	}

	static int luaB_registerClass(lua_State* L) {
		const char *self = luaL_optstring(L, 1, NULL);
		const char *base = luaL_optstring(L, 2, "");

		if (!self) {
			Axon::Errorf("luaB_registerClass: parameter error");
			return 0;
		}

		Axon::g_scriptSystem->registerClass(self, base);

		return 0;
	}

	static const luaL_Reg base_funcs[] = {
		{"print", luaB_print},
		{"dofile", luaB_dofile},
		{"registerClass", luaB_registerClass},
		{NULL, NULL}
	};

	// replace lua loader with ours loader reading virtual filesystem

	static int readable (const char *filename) {
		FILE *f = fopen(filename, "r");  /* try to open file */
		if (f == NULL) return 0;  /* open failed */
		fclose(f);
		return 1;
	}


	static const char *pushnexttemplate (lua_State *L, const char *path) {
		const char *l;
		while (*path == *LUA_PATHSEP) path++;  /* skip separators */
		if (*path == '\0') return NULL;  /* no more templates */
		l = strchr(path, *LUA_PATHSEP);  /* find next separator */
		if (l == NULL) l = path + strlen(path);
		lua_pushlstring(L, path, l - path);  /* template */
		return l;
	}

	static const char *findfile (lua_State *L, const char *name,
		const char *pname) {
			const char *path;
			name = luaL_gsub(L, name, ".", LUA_DIRSEP);
			lua_getfield(L, LUA_ENVIRONINDEX, pname);
			path = lua_tostring(L, -1);
			if (path == NULL)
				luaL_error(L, LUA_QL("package.%s") " must be a string", pname);
			lua_pushliteral(L, "");  /* error accumulator */
			while ((path = pushnexttemplate(L, path)) != NULL) {
				const char *filename;
				filename = luaL_gsub(L, lua_tostring(L, -1), LUA_PATH_MARK, name);
				lua_remove(L, -2);  /* remove path template */
				if (readable(filename))  /* does file exist and is readable? */
					return filename;  /* return that file name */
				lua_pushfstring(L, "\n\tno file " LUA_QS, filename);
				lua_remove(L, -2);  /* remove file name */
				lua_concat(L, 2);  /* add entry to possible error message */
			}
			return NULL;  /* not found */
	}


	static void loaderror (lua_State *L, const char *filename) {
		luaL_error(L, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s",
			lua_tostring(L, 1), filename, lua_tostring(L, -1));
	}


	static int loader_Lua (lua_State *L) {
#if 1
		const char *filename;
		const char *name = luaL_checkstring(L, 1);
		name = luaL_gsub(L, name, ".", "/");

		size_t filesize;
		char* filebuf;

		filesize = Axon::g_fileSystem->readFile(Axon::g_scriptSystem->getPackagePath() + name + ".lua", (void**)&filebuf);

		if (!filesize || !filebuf)
			return 1;

		int s = luaL_loadbuffer(L, filebuf, filesize, name);
		if (s)
			loaderror(L, name);

		return 1;

		filename = findfile(L, name, "path");

		if (filename == NULL) return 1;  /* library not found in this path */
		if (luaL_loadfile(L, filename) != 0)
			loaderror(L, filename);
		return 1;  /* library loaded successfully */
#else
		const char *filename;
		const char *name = luaL_checkstring(L, 1);
		filename = findfile(L, name, "path");
		if (filename == NULL) return 1;  /* library not found in this path */
		if (luaL_loadfile(L, filename) != 0)
			loaderror(L, filename);
		return 1;  /* library loaded successfully */
#endif
	}

	static void replaceLoader() {
		int top = lua_gettop(L);

		lua_getfield(L, LUA_GLOBALSINDEX, "package");
		AX_ASSERT(lua_istable(L, -1));
		lua_getfield(L, -1, "loaders");
		AX_ASSERT(lua_istable(L, -1));
		lua_pushcfunction(L, &loader_Lua);
		lua_getfield(L, LUA_GLOBALSINDEX, "package");
		lua_setfenv(L, -2);
		lua_rawseti(L, -2, 2);

		lua_settop(L, top);
	}

} // end extern "C"


namespace Axon {

	static inline void xPushString(lua_State* L, const String& s)
	{
		lua_pushlstring(L, s.c_str(), s.length());
	}

	static Variant xReadStack(lua_State* L, int index)
	{
		Variant ret;

		switch (lua_type(L,index)) {
		case LUA_TBOOLEAN:
			ret = lua_toboolean(L, index) ? true : false;
			break;
		case LUA_TNUMBER:
			ret = Variant(luaL_checknumber(L,index));
			break;
		case LUA_TSTRING:
			ret = Variant(String(luaL_checkstring( L, index)));
			break;
		case LUA_TTABLE:
			int tableindex;
			if (index > 0) {
				tableindex = index;
			} else {
				tableindex = lua_gettop(L) + index + 1;
			}
			ret = LuaTable(tableindex);
			break;
		case LUA_TUSERDATA:
//			ret = Variant((Object*)lua_unboxpointer(L,index));
			break;
		}
		return ret;
	}


	static void xPushObject(lua_State* L, Object* obj)
	{
#if 0
		lua_boxpointer(L, (void*)obj);
		luaL_getmetatable(L, OBJECT_MT);
		lua_setmetatable(L, -2);
#else
		lua_pushlightuserdata(L, obj);
		lua_rawget(L, LUA_REGISTRYINDEX);
#endif
	}

	static void xPushStack(lua_State* L, const Variant& val)
	{
		switch (val.type) {
		case Variant::kBool:
			lua_pushboolean(L, val.boolval);
			break;
		case Variant::kInt:
			lua_pushnumber(L,val.intval);
			break;
		case Variant::kFloat:
			lua_pushnumber(L,val.realval);
			break;
		case Variant::kString:
			xPushString(L, *val.str);
			break;
		case Variant::kObject:
			xPushObject(L, val.obj);
			break;
		case Variant::kVector3: {
			lua_newtable(L);
			int index = lua_gettop(L);
			LuaTable t(index);
			Vector3* v = (Vector3*)val.minibuf;
			t.set("x", v->x);
			t.set("y", v->y);
			t.set("z", v->z);
			break;
		}
		case Variant::kPoint:{
			lua_newtable(L);
			int index = lua_gettop(L);
			LuaTable t(index);
			Point* v = (Point*)val.minibuf;
			t.set("x", v->x);
			t.set("y", v->y);
			break;
		}
		case Variant::kRect:{
			lua_newtable(L);
			int index = lua_gettop(L);
			LuaTable t(index);
			Rect* v = (Rect*)val.minibuf;
			t.set("x", v->x);
			t.set("y", v->y);
			t.set("width" , v->width);
			t.set("height" , v->height);
			break;
		}
		case Variant::kColor:{
			lua_newtable(L);
			int index = lua_gettop(L);
			LuaTable t(index);
			Rgb* v = (Rgb*)val.minibuf;
			t.set("r", v->r/255.0f);
			t.set("g", v->g/255.0f);
			t.set("b", v->b/255.0f);
			break;
		}
		case Variant::kEmpty:
		default:
			lua_pushnil(L);
			break;
		}
	}

	static Object* xGetObject(lua_State* L, int index=1)
	{
		if (!lua_istable(L, index)) {
			return 0;
		}
		lua_pushliteral(L, "__object");
		lua_rawget(L, index);

		if (!lua_islightuserdata(L, -1)) {
			int type = lua_type(L, -1);
			lua_pop(L, 1);
			return 0;
		}
		Object* obj = (Object*)lua_touserdata(L, -1);
		lua_pop(L, 1);

		return obj;
	}

	static int xMetaCall(lua_State *L)
	{
		// func id
		luaL_checktype(L,lua_upvalueindex(1), LUA_TLIGHTUSERDATA);
		Member* member = (Member*)lua_topointer(L, lua_upvalueindex(1));

		// this object
#if 0
		luaL_checktype(L, 1, LUA_TUSERDATA);
		Object* obj = (Object*)lua_unboxpointer(L, 1);
#else
		Object* obj = xGetObject(L);
		if (!obj) {
			return 0;
		}
#endif
		// params
		VariantSeq stack;
		int need = member->getNumParams();
		int i; int count = 0;
		for (i=2; i<=lua_gettop(L); i++) {
			if (count == need)
				break;

			Variant p = xReadStack(L,i);
			stack.push_back(p);
			count++;
		}
		while (count < need) {
			stack.push_back(Variant());
			count++;
		}

		int retCount = member->invoke(obj, stack);

		int size = (int)stack.size();
		for (i=size-retCount; i<size; i++) {
			xPushStack(L, stack[i]);
		}

		return retCount;
	}

	static int xMetaIndex(lua_State *L)
	{
#if 0
		luaL_checktype(L, 1, LUA_TUSERDATA);
		Object* obj = (Object*)lua_unboxpointer(L, 1);
#else
		Object* obj = xGetObject(L);
		if (!obj) {
			return 0;
		}
#endif
		const char* name = luaL_checkstring(L,2);

		// Is Attribute ?
		Member* member = obj->findMember(name);
		if (!member)
			return 0;

		if (member->getType() == Member::kPropertyType) {
			Variant prop = member->getProperty(obj);
			xPushStack(L, prop);
			return 1;
		}

		// Is Method ?
		lua_pushlightuserdata(L, (void*)member);
		lua_pushcclosure(L, xMetaCall, 1);
		return 1;
	}

	static int xMetaNewIndex(lua_State *L)
	{
#if 0
		luaL_checktype(L, 1, LUA_TUSERDATA);
		Object* obj = (Object*)lua_unboxpointer(L, 1);
#else
		Object* obj = xGetObject(L);
		if (!obj) {
			return 0;
		}
#endif
		const char* name = luaL_checkstring( L, 2);

		Member* member = obj->findMember(name);

		if (!member) {
			lua_rawset(L, 1);
			return 0;
		}
		if (member->getType() == Member::kPropertyType) {
			Variant val = xReadStack(L,3);
			member->setProperty(obj, val);
		}

		return 0;
	}

	// find a scoped variable in global
	static bool xGetGlobalScoped(lua_State* L, const char* name)
	{
		char buf[32];
		int num = 0;

		// push global in stack first
		lua_pushvalue(L, LUA_GLOBALSINDEX);

		if (!name || !name[0]) {
			return true;
		}

		while (1) {
			int c = *name++;

			if (isalpha(c) || c == '_' || (isdigit(c) && num != 0)) {
				buf[num++] = c;
				continue;
			}

			if (c == '.' || c == 0) {
				if (num  && lua_istable(L,-1)) {
					buf[num] = 0;
					lua_getfield(L, -1, buf);
					lua_remove(L, -2);
				} else {
					return false;
				}

				if (c == 0) {
					break;
				}

				num = 0;
				continue;
			}

			// not known char, error
			return false;
		}

		return true;
	}

	// find a scoped variable in stack top's table, table is popped
	static bool xGetScoped(lua_State* L, const char* name)
	{
		char buf[32];
		int num = 0;

		if (!name || !name[0]) {
			return false;
		}

		while (1) {
			int c = *name++;

			if (isalpha(c) || c == '_' || (isdigit(c) && num != 0)) {
				buf[num++] = c;
				AX_ASSERT(num<s2i(ArraySize(buf)));
				continue;
			}

			if (c == '.' || c == 0) {
				if (num  && lua_istable(L,-1)) {
					buf[num] = 0;
					lua_getfield(L, -1, buf);
					lua_remove(L, -2);
				} else {
					return false;
				}

				if (c == 0) {
					break;
				}

				num = 0;
				continue;
			}

			// not known char, error
			return false;
		}

		return true;
	}

	static bool xSetScoped(lua_State* L, const char* name, const Variant& val)
	{
		char buf[32];
		int num = 0;

		if (!name || !name[0]) {
			return false;
		}

		while (1) {
			int c = *name++;

			if (isalpha(c) || c == '_' || (isdigit(c) && num != 0)) {
				buf[num++] = c;
				AX_ASSERT(num<s2i(ArraySize(buf)));
				continue;
			}

			if (c == '.') {
				if (num  && lua_istable(L,-1)) {
					buf[num] = 0;
					lua_getfield(L, -1, buf);
					lua_remove(L, -2);
				} else {
					return false;
				}

				num = 0;
				continue;
			}

			if (c == 0) {
				buf[num] = 0;
				xPushStack(L,val);
				lua_setfield(L,-2,buf);
				return true;
			}

			// not known char, error
			return false;
		}

		return true;
	}

	static int xPcall(lua_State* L, int numarg, int numresult)
	{
		int s = lua_pcall(L, numarg, numresult, 0);
		return xReport(L, s, 0);
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

	Variant LuaTable::get(const String& n) const
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

	void LuaTable::set(const String& n, const Variant& v)
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

	Object* LuaTable::toObject() const
	{
		beginRead();
		Object* result = get("__object");
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

	bool LuaTable::nextIterator(Variant& k, Variant& v) const
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

	//--------------------------------------------------------------------------
	// class Object
	//--------------------------------------------------------------------------

	MetaInfo* Object::m_metaInfo = nullptr;

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
	}

#if 0
	Object::Object(const String& objname)
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

		set_objectName(objname);
	}
#endif

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

	Member* Object::findMember(const char* name) const
	{
		MetaInfo* typeinfo = getMetaInfo();

		while (typeinfo) {
			Member* member = typeinfo->findMember(name);
			if (member) return member;
			typeinfo = typeinfo->getBaseTypeInfo();
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

	MetaInfo* Object::getMetaInfo() const
	{
		if (!m_metaInfo) {
			Object::registerMetaInfo();
			AX_ASSERT(m_metaInfo);
		}

		return m_metaInfo;
	}

	MetaInfo* Object::registerMetaInfo()
	{
		if (!m_metaInfo) {
			m_metaInfo = new MetaInfo_<Object>("Object", nullptr);
			m_metaInfo->addProperty("objectName", &Object::get_objectName, &Object::set_objectName);

			g_scriptSystem->registerType(m_metaInfo);
		}
		return m_metaInfo;
	}


	void Object::set_objectName(const String& name)
	{
		setObjectName(name, false);
	}

	String Object::get_objectName() const
	{
		return m_objectName;
	}


	bool Object::inherits(const char* cls) const
	{
		MetaInfo* typeinfo = getMetaInfo();

		for (; typeinfo; typeinfo=typeinfo->getBaseTypeInfo()) {
			if (Strequ(cls, typeinfo->getTypeName())) {
				return true;
			}
		}

		return false;
	}

	Variant Object::getProperty(const char* name) const
	{
		Member* m = findMember(name);
		if (!m || !m->isProperty()) {
			return Variant();
		}

		return m->getProperty(this);
	}

	bool Object::setProperty(const char* name, const Variant& value)
	{
		Member* m = findMember(name);
		if (!m || !m->isProperty() || m->isConst()) {
			return false;
		}

		m->setProperty(this, value);

		return true;
	}

	bool Object::setProperty(const char* name, const char* value)
	{
		Member* member = findMember(name);

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

	void Object::writeProperties(File* f, int indent) const
	{
		String indstr(indent*2, ' ');
#define INDENT if (indent) f->printf("%s", indstr.c_str());

		// write properties
		MetaInfo* typeinfo = getMetaInfo();

		while (typeinfo) {
			const MemberSeq& members = typeinfo->getMembers();

			AX_FOREACH(Member* m, members) {
				if (!m->isProperty()) {
					continue;
				}

				if (m->isConst()) {
					continue;
				}

				INDENT; f->printf("  %s=\"%s\"\n", m->getName(), getProperty(m->getName()).toString().c_str());
			}

			typeinfo = typeinfo->getBaseTypeInfo();
		}

		// write script properties
		const ClassInfo* classinfo = getClassInfo();
		if (!classinfo) return;

		const ScriptPropSeq& props = classinfo->m_scriptPropSeq;

		AX_FOREACH(ScriptProp* m, props) {
			if (m->getPropKind() == ScriptProp::kGroup)
				continue;

			INDENT; f->printf("  %s=\"%s\"\n", m->getName(), m->getProperty(this).toString().c_str());
		}

#undef INDENT
	}

	void Object::readProperties(const TiXmlElement* node)
	{
		const TiXmlAttribute* attr = node->FirstAttribute();

		for (; attr; attr = attr->Next()) {
			this->setProperty(attr->Name(), attr->Value());
		}
	}

	void Object::copyPropertiesFrom(const Object* rhs)
	{
		// write properties
		MetaInfo* typeinfo = rhs->getMetaInfo();

		while (typeinfo) {
			// don't copy objectname
			if (typeinfo == Object::m_metaInfo)
				break;

			const MemberSeq& members = typeinfo->getMembers();

			AX_FOREACH(Member* m, members) {
				if (!m->isProperty()) {
					continue;
				}

				if (m->isConst()) {
					continue;
				}

				setProperty(m->getName(), m->getProperty(rhs));
			}

			typeinfo = typeinfo->getBaseTypeInfo();
		}

		// write script properties
		const ClassInfo* classinfo = getClassInfo();
		if (!classinfo) return;

		const ScriptPropSeq& props = classinfo->m_scriptPropSeq;

		AX_FOREACH(ScriptProp* m, props) {
			if (m->getPropKind() == ScriptProp::kGroup)
				continue;

			setProperty(m->getName(), m->getProperty(rhs));
		}
	}


	void Object::initClassInfo(const ClassInfo* ci)
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

	void Object::setObjectName(const String& name, bool force)
	{
		if (m_objectName == name && !force) {
			return;
		}

		resetObjectName();

		m_objectName = name;
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

	void Object::invokeCallback(const String& callback)
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

	void Object::invokeCallback(const String& callback, const Variant& param)
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

	void Object::invokeMethod(const char* name, const Variant& arg1)
	{
		Member* member = findMember(name);

		if (!member || !member->isMethod()) {
			return;
		}

		VariantSeq vs;
		vs.push_back(arg1);
		member->invoke(this, vs);
	}

	void Object::invokeMethod(const char* name, const Variant& arg1, const Variant& arg2)
	{
		Member* member = findMember(name);

		if (!member || !member->isMethod()) {
			return;
		}

		VariantSeq vs;
		vs.push_back(arg1);
		vs.push_back(arg2);
		member->invoke(this, vs);
	}

	bool Object::isClass(const char* classname) const
	{
		if (!m_classInfo) {
			return false;
		}

		return m_classInfo->m_className == classname;
	}

	void Object::setRuntime(const char* name, const Variant& val)
	{
		int top = lua_gettop(L);

		lua_pushlightuserdata(L, this);
		lua_rawget(L, LUA_REGISTRYINDEX);

		xPushStack(L, val);

		lua_setfield(L, -2, name);

		lua_settop(L, top);

		return;
	}

	Variant Object::getRuntime(const char* name)
	{
		int top = lua_gettop(L);

		lua_pushlightuserdata(L, this);
		lua_rawget(L, LUA_REGISTRYINDEX);

		lua_getfield(L, -2, name);
		Variant result = xReadStack(L,-1);

		if (result.type == Variant::kTable) {
			Errorf("cann't get Lua table in getRuntime");
		}

		lua_settop(L, top);

		return result;
	}

	//--------------------------------------------------------------------------
	// class ScriptSystem
	//--------------------------------------------------------------------------
	struct Connection {
		Object* obj;
		String callname;
	};

	typedef List<Connection> ConnectionSeq;
	typedef Dict<Object*,Dict<String, ConnectionSeq>> Connections;

	ScriptSystem::ScriptSystem()
	{
		m_isReading = false;
		m_readTop = 0;
	}

	ScriptSystem::~ScriptSystem()
	{}

	void ScriptSystem::initialize()
	{
		Printf(_("Initializing ScriptSystem...\n"));

		L = lua_open();

		if (!L) {
			Errorf(_("ScriptSystem::initialize: cann't open Lua library\n"));
		} else {
			Printf(_("..opened Lua library\n"));
		}

		luaL_openlibs(L);

		replaceLoader();

		luaL_register(L, "_G", base_funcs);
		lua_pop(L, 1);

		Printf(_("..opened LuaLib library\n"));

		// set package path
		m_packagePath = "scripts/";
		String packageFind = FileSystem::getDefaultDataPath() + "/" + m_packagePath + "?.lua";

		lua_pushliteral(L, "package");
		lua_rawget(L, LUA_GLOBALSINDEX);
		AX_ASSERT(lua_istable(L, -1));
		lua_pushliteral(L, "path");
//		lua_gettable(L, -2);
//		AX_ASSERT(lua_isstring(L, -1));
		xPushString(L, packageFind);
		lua_settable(L, -3);
		lua_pop(L, 1);

		luaL_newmetatable(L, OBJECT_MT);

		lua_pushliteral(L, "__index");
		lua_pushcclosure(L, xMetaIndex, 0);
		lua_rawset(L, -3);

		lua_pushliteral(L, "__newindex");
		lua_pushcclosure(L, xMetaNewIndex, 0);
		lua_rawset(L, -3);

		lua_pop(L, 1);

		AX_ASSERT(lua_gettop(L) == 0);

		executeFile("Start.lua");

		Printf(_("..created userdata object for engine object\n"));

		Printf(_("Initialized ScriptSystem\n"));
	}

	void ScriptSystem::finalize()
	{
		lua_close(L);
	}

#if 0
	void ScriptSystem::addObject(const String& name, Object* obj)
	{
		xPushObject(L,obj);
		xPushString(L, name);
		lua_pushvalue(L, -2);
		lua_rawset(L, LUA_GLOBALSINDEX);
		lua_remove(L, lua_gettop(L));
	}

	void ScriptSystem::removeObject(const String& name) {}

	void ScriptSystem::removeObject(Object* obj) {}
#endif

	void ScriptSystem::executeString(const String& text)
	{
		int s = luaL_loadbuffer(L, text.c_str(), text.size(), 0);
		if (s==0) {
			// execute Lua program
			s = lua_pcall(L, 0, LUA_MULTRET, 0);
		}
		xReport(L, s, 0);
	}

	void ScriptSystem::executeString(const char* text)
	{
		int s = luaL_loadstring(L, text);
		if (s==0) {
			// execute Lua program
			s = lua_pcall(L, 0, LUA_MULTRET, 0);
		}
		xReport(L, s, 0);
	}

	void ScriptSystem::executeFile(const String& filename)
	{
		size_t filesize;
		char* filebuf;

		filesize = g_fileSystem->readFile(m_packagePath + filename, (void**)&filebuf);

		if (!filesize || !filebuf)
			return;

		int s = luaL_loadbuffer(L, filebuf, filesize, filename.c_str());
		if (s==0) {
			// execute Lua program
			s = lua_pcall(L, 0, LUA_MULTRET, 0);
		}
		xReport(L, s, filename.c_str());
	}

	bool ScriptSystem::invokeLuaMethod(const char* methodName, VariantSeq& stack, int nResult)
	{
		int top = lua_gettop(L);

		// find method
		bool v = xGetGlobalScoped(L, methodName);
		if (!v) goto errquit;

		if (!lua_isfunction(L,-1)) goto errquit;
	
		int argCount = (int)stack.size();
		int i;

		for (i = 0; i < argCount; i++) {
			xPushStack(L, stack[i]);
		}

		lua_pcall(L, argCount, nResult, 0);

		for (i = 0; i < nResult; i++) {
			xReadStack(L, i);
		}

		lua_settop(L, top);
		return true;

errquit:
		lua_settop(L, top);
		return false;
	}

	bool ScriptSystem::invokeLuaMethod(const char* method, Variant& arg1)
	{
		VariantSeq vseq;
		vseq.push_back(arg1);

		return invokeLuaMethod(method, vseq, 0);
	}

	bool ScriptSystem::invokeLuaScoped(const char *text, Axon::VariantSeq &stack, int nResult)
	{
		String fullName(text);
		String::size_type idx_end = fullName.find('.');
		if (idx_end == String.npos) {
			return invokeLuaMethod(text,stack,nResult);
		} else {
			String objectname = fullName.substr(0,idx_end);
			String membername = fullName.substr(idx_end+1,fullName.size());
			xPushString(L, objectname);
			lua_rawget(L, LUA_GLOBALSINDEX);
			Object* obj = xGetObject(L, lua_gettop(L));
			lua_pop(L,1);
			if (obj) {								
				Member* mb = obj->findMember(membername.c_str());
				if (mb && mb->isMethod())
					mb->invoke(obj,stack);
			} else {
				lua_getglobal(L,objectname.c_str());
				Variant variant = xReadStack(L,-1);
				if (Variant::kTable == variant.type) {
					xPushString(L, membername);
					lua_gettable(L, -2);
					lua_pushvalue(L, -2);
					int argCount = (int)stack.size();
					int i;

					for (i = 0; i < argCount; i++) {
						xPushStack(L, stack[i]);
					}

					lua_pcall(L, argCount+1, nResult, 0);

					for (i = 0; i < nResult; i++) {
						xReadStack(L, i);					
					}
					lua_pop(L,1);
				}				
			}				
		}
		return true;
	}

	String ScriptSystem::generateLuaString(const String& text)
	{
		String result = text;
		String::size_type idx_end = text.find('.');
		if (idx_end == String.npos){
			int id = lua_gettop(L);
			lua_getglobal(L,text.c_str());
			Variant variant = xReadStack(L,-1);
			if (variant.type == Variant::kString)
				result = variant.toString();
			lua_pop(L,1);
			id = lua_gettop(L);
		}else{
			int id = lua_gettop(L);
			String objectname = text.substr(0,idx_end);
			String membername = text.substr(idx_end+1,text.size());
			xPushString(L, objectname);
			lua_rawget(L, LUA_GLOBALSINDEX);
			Object* obj = xGetObject(L, lua_gettop(L));
			lua_pop(L,1);
			id = lua_gettop(L);
			if (obj){								
				Variant var = obj->getProperty(membername.c_str());
				if (var.type == Variant::kString)
					result = var;						
			}else{
				lua_getglobal(L,objectname.c_str());
				Variant variant = xReadStack(L,-1);
				
				if (variant.type == Variant::kTable){
					xPushString(L, membername);
					lua_gettable(L, -2);
					Variant res = xReadStack(L,-1);
					if (res.type == Variant::kString)
						result = res.toString();
					lua_pop(L,1);
				}
				lua_pop(L,1);
				id = lua_gettop(L);
			}
		}
		return result;
	}

	inline String xRemoveScope(const String& str)
	{
		size_t pos = str.rfind('.');
		if (pos == String::npos || pos >= str.length()-1) {
			return str;
		}

		return str.substr(pos+1, str.length()-pos-1);
	}

	inline String xRemoveIndex(const String& str)
	{
		// first get index from str
		String::const_reverse_iterator it = str.rbegin();

		int count = 0;
		for (; it != str.rend(); ++it) {
			if (!isdigit(*it)) {
				break;
			}
			count++;
		}
		if (!count)
			return str;

		if (it == str.rend())
			return str;

		++it;

		if (*it != '_')
			return str;

		return str.substr(0, str.size() - count - 1);
	}

	int ScriptSystem::getNameIndex(const String& str) const
	{
		StringIntDict::const_iterator it = m_objectNameGen.find(xRemoveIndex(str));
		if (it == m_objectNameGen.end())
			return 0;
		return it->second;
	}

	void ScriptSystem::updateNameIndex(const String& str)
	{
		// first get index from str
		String::const_reverse_iterator it = str.rbegin();

		int count = 0;
		for (; it != str.rend(); ++it) {
			if (!isdigit(*it)) {
				break;
			}
			count++;
		}
		const char* p = &*it;

		bool noindex = false;
		if (!count) {
			noindex = true;
		}

		if (it == str.rend()) {
			noindex = true;
		}

		if (*it != '_') {
			noindex = true;
		}

		int index = 0;
		String name(str);

		if (!noindex) {
			index = atoi(p + 1);
			name = String(&str[0], p);
		}

		// check if need update
		int curindex = getNameIndex(name);
		if (index < curindex)
			return;

		m_objectNameGen[name] = index+1;
	}

	int ScriptSystem::nextNameIndex(const String& str)
	{
		return m_objectNameGen[str]++;
	}

	String ScriptSystem::generateObjectName(const String& str)
	{
		String noindex = xRemoveScope(xRemoveIndex(str));

		int index = nextNameIndex(noindex);

		if (!index)
			return noindex;

		String result;
		StringUtil::sprintf(result, "%s_%d", noindex.c_str(), index);
		return result;
	}

	void ScriptSystem::registerType(MetaInfo* typeinfo)
	{
		m_typeInfoReg[typeinfo->getTypeName()] = typeinfo;

		setTypeInfoToClassInfo(typeinfo->m_typeName, typeinfo);
	}

	Object* ScriptSystem::createObject(const char* classname)
	{
		ClassInfoDict::const_iterator cit = m_classInfoReg.find(classname);
		if (cit != m_classInfoReg.end()) {
			ClassInfo* ci = cit->second;

			if (!ci->m_typeInfo) {
				Errorf("can't find type info");
			}

			// create object
			Object* obj = ci->m_typeInfo->createObject();
			obj->initClassInfo(ci);

			obj->invoke_onInit();

			return obj;
		}

		TypeInfoDict::const_iterator it = m_typeInfoReg.find(classname);

		if (it == m_typeInfoReg.end()) {
			return nullptr;
		}

		Object* obj = it->second->createObject();
		obj->invoke_onInit();
		return obj;
	}

	Object* ScriptSystem::cloneObject(const Object* obj)
	{
		Object* result = 0;
		
		if (obj->getClassInfo())
			result = createObject(obj->getClassInfo()->m_className.c_str());

		if (!result)
			result = obj->getMetaInfo()->createObject();

		if (!result)
			Errorf("can't create object");

		// copy properties
		result->copyPropertiesFrom(obj);

		return result;
	}
	
	Object* ScriptSystem::findObject(const String& objectname)
	{
		xPushString(L, objectname);
		lua_rawget(L, LUA_GLOBALSINDEX);
		Object* result = xGetObject(L, lua_gettop(L));
		lua_pop(L,1);
		return result;
	}

	void ScriptSystem::beginRead()
	{
		if (m_isReading) {
			Errorf("already in reading");
		}

		m_isReading = true;
		m_readTop = lua_gettop(L);
	}

	Variant ScriptSystem::readField(const char* objname, const char* fieldname)
	{
		if (!m_isReading) {
			Errorf("not in reading");
		}

		Variant result;
		xGetGlobalScoped(L, objname);
		if (!fieldname) {
			return result;
		} else {
			if (lua_istable(L, -1)) {
				lua_getfield(L, -1, fieldname);
				lua_remove(L, -2);
			}
		}

		result = xReadStack(L, -1);
		return result;
	}

	Variant ScriptSystem::readField(const char* objname)
	{
		if (!m_isReading) {
			Errorf("not in reading");
		}

		Variant result;
		xGetGlobalScoped(L, objname);
		result = xReadStack(L, -1);
		return result;
	}

	void ScriptSystem::endRead()
	{
		if (!m_isReading) {
			Errorf("not in reading");
		}

		lua_settop(L,m_readTop);
		m_isReading = false;
	}

	Variant ScriptSystem::readFieldImmediately(const char* objname, const char* fieldname)
	{
		Variant result;

		xGetGlobalScoped(L, objname);
		if (!fieldname) {
		} else {
			if (lua_istable(L, -1)) {
				lua_getfield(L, -1, fieldname);
				lua_remove(L, -2);
			}
		}

		result = xReadStack(L, -1);
		lua_pop(L, 1);
		return result;
	}

	void ScriptSystem::registerClass(const String& self, const String& base)
	{
		ClassInfoDict::iterator it = m_classInfoReg.find(self);

		if (it != m_classInfoReg.end()) {
			Errorf("Class already registered");
		}

		ClassInfo* classInfo = new ClassInfo;
		classInfo->m_className = self;
		classInfo->m_metaName = base;
		classInfo->m_typeInfo = 0;
		m_classInfoReg[self] = classInfo;

		classInfo->initScriptProps();
	}

	void ScriptSystem::setTypeInfoToClassInfo(const String& name, MetaInfo* ti)
	{
#if 0
		// if can't find class info, create one
		ClassInfoDict::iterator it = m_classInfoReg.find(name);
		if (it == m_classInfoReg.end()) {
			ClassInfo* ci = new ClassInfo;
			ci->m_className = name;
			ci->m_baseClassName = ti->getBaseTypeInfo()->getTypeName();
			ci->m_typeInfo = ti;

			m_classInfoReg[name] = ci;
		}
#endif
		ClassInfoDict::iterator it = m_classInfoReg.begin();

		for (; it != m_classInfoReg.end(); ++it) {
			ClassInfo* ci = it->second;
			if (ci->m_className == name) {
				ci->m_typeInfo = ti;
			} else if (ci->m_metaName == name) {
				ci->m_typeInfo = ti;
				setTypeInfoToClassInfo(ci->m_className, ti);
			}
		}
	}

	void ScriptSystem::getClassList(const char* prefix, bool sort, StringSeq& result) const
	{
		ClassInfoDict::const_iterator it = m_classInfoReg.begin();
		size_t prefixlen = 0;
		
		if (prefix) {
			prefixlen = strlen(prefix);
		}

		for (; it != m_classInfoReg.end(); ++it) {
			ClassInfo* ci = it->second;
			if (prefixlen) {
				if (strncmp(prefix, ci->m_className.c_str(), prefixlen) != 0) {
					continue;
				}

				result.push_back(ci->m_className.c_str() + prefixlen);
			} else {
				result.push_back(ci->m_className);
			}
		}

		if (sort) {
			std::sort(result.begin(), result.end(), std::less<String>());
		}
	}

	bool lesser(const ScriptProp* a, const ScriptProp* b)
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
			ScriptProp* sa = new ScriptProp();

			sa->m_realName = lua_tostring(L, -2);
			sa->m_group = 0;
			sa->init();
			m_scriptProps[sa->m_showName] = sa;
			m_scriptPropSeq.push_back(sa);

			if (sa->m_propKind == ScriptProp::kGroup) {
				lua_pushnil(L);
				while (lua_next(L,-2)) {
					AX_ASSERT(lua_isstring(L,-2));
					ScriptProp* child = new ScriptProp();

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

	Variant ClassInfo::getField(const String& field) const
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


	int ScriptProp::checkTableKind(Variant& result)
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

			const char* name = lua_tostring(L, -2);

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

	void ScriptProp::setProperty(Object* obj, const Variant& val)
	{
		if (m_propKind == kGroup)
			return;

		int top = lua_gettop(L);

		lua_pushlightuserdata(L, const_cast<Object*>(obj));
		lua_rawget(L, LUA_REGISTRYINDEX);

		xSetScoped(L, ("Properties." + m_realName).c_str(), val);

		lua_settop(L, top);
	}

	Variant ScriptProp::getProperty(const Object* obj)
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

} // namespace Axon

