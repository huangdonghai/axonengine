#include "script_p.h"

/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#define OBJECT_MT "__object_mt"
#define OBJECT_NAME "__object"

AX_BEGIN_NAMESPACE

lua_State *L;
SquirrelVM *g_mainVM;

static void l_message(const char *pname, const char *msg)
{
	if (pname) Axon::Debugf("%s: ", pname);
	Axon::Errorf("%s\n", msg);
}


int xReport(lua_State *L, int status, const char *progname)
{
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		l_message(progname, msg);
		lua_pop(L, 1);
	}
	return status;
}


static int luaB_print (lua_State *L)
{
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

static int luaB_dofile (lua_State *L)
{
	const char *fname = luaL_optstring(L, 1, NULL);
	int n = lua_gettop(L);
#if 0
	if (luaL_loadfile(L, fname) != 0) lua_error(L);
#else
	size_t filesize;
	char *filebuf;

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

static int luaB_registerClass(lua_State *L)
{
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

static int readable (const char *filename)
{
	FILE *f = fopen(filename, "r");  /* try to open file */
	if (f == NULL) return 0;  /* open failed */
	fclose(f);
	return 1;
}


static const char *pushnexttemplate (lua_State *L, const char *path)
{
	const char *l;
	while (*path == *LUA_PATHSEP) path++;  /* skip separators */
	if (*path == '\0') return NULL;  /* no more templates */
	l = strchr(path, *LUA_PATHSEP);  /* find next separator */
	if (l == NULL) l = path + strlen(path);
	lua_pushlstring(L, path, l - path);  /* template */
	return l;
}

static const char *findfile (lua_State *L, const char *name, const char *pname)
{
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


static void loaderror (lua_State *L, const char *filename)
{
	luaL_error(L, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s",
		lua_tostring(L, 1), filename, lua_tostring(L, -1));
}


static int loader_Lua (lua_State *L)
{
#if 1
	const char *filename;
	const char *name = luaL_checkstring(L, 1);
	name = luaL_gsub(L, name, ".", "/");

	size_t filesize;
	char *filebuf;

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

static void replaceLoader()
{
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


void xPushString(lua_State *L, const String &s)
{
	lua_pushlstring(L, s.c_str(), s.length());
}

Variant xReadStack(lua_State *L, int index)
{
	Variant ret;

	switch (lua_type(L,index)) {
	case LUA_TBOOLEAN:
		ret = lua_toboolean(L, index) ? true : false;
		break;
	case LUA_TNUMBER:
		ret = Variant((float)luaL_checknumber(L,index));
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


static void xPushObject(lua_State *L, Object *obj)
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

void xPushStack(lua_State *L, const Variant &val)
{
	switch (val.getType()) {
	case Variant::kBool:
		lua_pushboolean(L, (bool)val);
		break;
	case Variant::kInt:
		lua_pushnumber(L, (int)val);
		break;
	case Variant::kFloat:
		lua_pushnumber(L, (float)val);
		break;
	case Variant::kString:
		xPushString(L, (String)val);
		break;
	case Variant::kObject:
		xPushObject(L, (Object *)val);
		break;
	case Variant::kVector3: {
		lua_newtable(L);
		int index = lua_gettop(L);
		LuaTable t(index);
		const Vector3 &v = val.ref<Vector3>();
		t.set("x", v.x);
		t.set("y", v.y);
		t.set("z", v.z);
		break;
	}
	case Variant::kPoint:{
		lua_newtable(L);
		int index = lua_gettop(L);
		LuaTable t(index);
		Point *v = (Point*)val.getPtr();
		t.set("x", v->x);
		t.set("y", v->y);
		break;
	}
	case Variant::kRect:{
		lua_newtable(L);
		int index = lua_gettop(L);
		LuaTable t(index);
		Rect *v = (Rect*)val.getPtr();
		t.set("x", v->x);
		t.set("y", v->y);
		t.set("width" , v->width);
		t.set("height" , v->height);
		break;
	}
	case Variant::kColor3:{
		lua_newtable(L);
		int index = lua_gettop(L);
		LuaTable t(index);
		Color3 *v = (Color3*)val.getPtr();
		t.set("r", v->r/255.0f);
		t.set("g", v->g/255.0f);
		t.set("b", v->b/255.0f);
		break;
	}
	case Variant::kVoid:
	default:
		lua_pushnil(L);
		break;
	}
}

static Object *xGetObject(lua_State *L, int index=1)
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
	Object *obj = (Object*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	return obj;
}

static int xMetaCall(lua_State *L)
{
	// func id
	luaL_checktype(L,lua_upvalueindex(1), LUA_TLIGHTUSERDATA);
	Member *member = (Member*)lua_topointer(L, lua_upvalueindex(1));

	// this object
#if 0
	luaL_checktype(L, 1, LUA_TUSERDATA);
	Object *obj = (Object*)lua_unboxpointer(L, 1);
#else
	Object *obj = xGetObject(L);
	if (!obj) {
		return 0;
	}
#endif
	// params
	VariantSeq stack;
	int need = member->argc();
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
	Object *obj = (Object*)lua_unboxpointer(L, 1);
#else
	Object *obj = xGetObject(L);
	if (!obj) {
		return 0;
	}
#endif
	const char *name = luaL_checkstring(L,2);

	// Is Attribute ?
	Member *member = obj->findMember(name);
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
	Object *obj = (Object*)lua_unboxpointer(L, 1);
#else
	Object *obj = xGetObject(L);
	if (!obj) {
		return 0;
	}
#endif
	const char *name = luaL_checkstring( L, 2);

	Member *member = obj->findMember(name);

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
bool xGetGlobalScoped(lua_State *L, const char *name)
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
bool xGetScoped(lua_State *L, const char *name)
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

bool xSetScoped(lua_State *L, const char *name, const Variant &val)
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

int xPcall(lua_State *L, int numarg, int numresult)
{
	int s = lua_pcall(L, numarg, numresult, 0);
	return xReport(L, s, 0);
}


//--------------------------------------------------------------------------
// class ScriptSystem
//--------------------------------------------------------------------------
struct Connection {
	Object *obj;
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
//	lua_gettable(L, -2);
//	AX_ASSERT(lua_isstring(L, -1));
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

	g_mainVM = new SquirrelVM();
	_INIT_CLASS(Vector3);
	_INIT_CLASS(Color3);
	_INIT_CLASS(Point);
	_INIT_CLASS(Rect);
	_INIT_CLASS(Matrix);

	Printf(_("Initialized ScriptSystem\n"));
}

void ScriptSystem::finalize()
{
	SafeDelete(g_mainVM);

	lua_close(L);
}

#if 0
void ScriptSystem::addObject(const String &name, Object *obj)
{
	xPushObject(L,obj);
	xPushString(L, name);
	lua_pushvalue(L, -2);
	lua_rawset(L, LUA_GLOBALSINDEX);
	lua_remove(L, lua_gettop(L));
}

void ScriptSystem::removeObject(const String &name) {}

void ScriptSystem::removeObject(Object *obj) {}
#endif

void ScriptSystem::executeString(const String &text)
{
	int s = luaL_loadbuffer(L, text.c_str(), text.size(), 0);
	if (s==0) {
		// execute Lua program
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	}
	xReport(L, s, 0);
}

void ScriptSystem::executeString(const char *text)
{
	int s = luaL_loadstring(L, text);
	if (s==0) {
		// execute Lua program
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
	}
	xReport(L, s, 0);
}


void ScriptSystem::executeLine( const char *text )
{
	SquirrelObject bytecode = g_mainVM->compileBuffer(text);
	g_mainVM->runScript(bytecode);
}


void ScriptSystem::executeFile(const String &filename)
{
	size_t filesize;
	char *filebuf;

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

bool ScriptSystem::invokeLuaMethod(const char *methodName, VariantSeq &stack, int nResult)
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

bool ScriptSystem::invokeLuaMethod(const char *method, Variant &arg1)
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
		Object *obj = xGetObject(L, lua_gettop(L));
		lua_pop(L,1);
		if (obj) {								
			Member *mb = obj->findMember(membername.c_str());
			if (mb && mb->isMethod())
				mb->invoke(obj,stack);
		} else {
			lua_getglobal(L,objectname.c_str());
			Variant variant = xReadStack(L,-1);
			if (Variant::kTable == variant.getType()) {
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

String ScriptSystem::generateLuaString(const String &text)
{
	String result = text;
	String::size_type idx_end = text.find('.');
	if (idx_end == String.npos){
		int id = lua_gettop(L);
		lua_getglobal(L,text.c_str());
		Variant variant = xReadStack(L,-1);
		if (variant.getType() == Variant::kString)
			result = variant.toString();
		lua_pop(L,1);
		id = lua_gettop(L);
	}else{
		int id = lua_gettop(L);
		String objectname = text.substr(0,idx_end);
		String membername = text.substr(idx_end+1,text.size());
		xPushString(L, objectname);
		lua_rawget(L, LUA_GLOBALSINDEX);
		Object *obj = xGetObject(L, lua_gettop(L));
		lua_pop(L,1);
		id = lua_gettop(L);
		if (obj){								
			Variant var = obj->getProperty(membername.c_str());
			if (var.getType() == Variant::kString)
				result = var;						
		}else{
			lua_getglobal(L,objectname.c_str());
			Variant variant = xReadStack(L,-1);
			
			if (variant.getType() == Variant::kTable){
				xPushString(L, membername);
				lua_gettable(L, -2);
				Variant res = xReadStack(L,-1);
				if (res.getType() == Variant::kString)
					result = res.toString();
				lua_pop(L,1);
			}
			lua_pop(L,1);
			id = lua_gettop(L);
		}
	}
	return result;
}

inline String xRemoveScope(const String &str)
{
	size_t pos = str.rfind('.');
	if (pos == String::npos || pos >= str.length()-1) {
		return str;
	}

	return str.substr(pos+1, str.length()-pos-1);
}

inline String xRemoveIndex(const String &str)
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

int ScriptSystem::getNameIndex(const String &str) const
{
	StringIntDict::const_iterator it = m_objectNameGen.find(xRemoveIndex(str));
	if (it == m_objectNameGen.end())
		return 0;
	return it->second;
}

void ScriptSystem::updateNameIndex(const String &str)
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
	const char *p = &*it;

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

int ScriptSystem::nextNameIndex(const String &str)
{
	return m_objectNameGen[str]++;
}

String ScriptSystem::generateObjectName(const String &str)
{
	String noindex = xRemoveScope(xRemoveIndex(str));

	int index = nextNameIndex(noindex);

	if (!index)
		return noindex;

	String result;
	StringUtil::sprintf(result, "%s_%d", noindex.c_str(), index);
	return result;
}

void ScriptSystem::registerType(MetaInfo *metainfo)
{
	m_typeInfoReg[metainfo->getName()] = metainfo;

	linkMetaInfoToClassInfo(metainfo);
}

Object *ScriptSystem::createObject(const char *classname)
{
	ClassInfoDict::const_iterator cit = m_classInfoReg.find(classname);
	if (cit != m_classInfoReg.end()) {
		ClassInfo *ci = cit->second;

		if (!ci->m_typeInfo) {
			Errorf("can't find type info");
		}

		// create object
		Object *obj = ci->m_typeInfo->createObject();
		obj->initClassInfo(ci);

		obj->invoke_onInit();

		return obj;
	}

	TypeInfoDict::const_iterator it = m_typeInfoReg.find(classname);

	if (it == m_typeInfoReg.end()) {
		return nullptr;
	}

	Object *obj = it->second->createObject();
	obj->invoke_onInit();
	return obj;
}

Object *ScriptSystem::cloneObject(const Object *obj)
{
	Object *result = 0;
	
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

Object *ScriptSystem::findObject(const String &objectname)
{
	xPushString(L, objectname);
	lua_rawget(L, LUA_GLOBALSINDEX);
	Object *result = xGetObject(L, lua_gettop(L));
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

Variant ScriptSystem::readField(const char *objname, const char *fieldname)
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

Variant ScriptSystem::readField(const char *objname)
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

Variant ScriptSystem::readFieldImmediately(const char *objname, const char *fieldname)
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

void ScriptSystem::registerClass(const String &self, const String &base)
{
	ClassInfoDict::iterator it = m_classInfoReg.find(self);

	if (it != m_classInfoReg.end()) {
		Errorf("Class already registered");
	}

	ClassInfo *classInfo = new ClassInfo;
	classInfo->m_className = self;
	classInfo->m_metaName = base;
	classInfo->m_typeInfo = 0;
	m_classInfoReg[self] = classInfo;

	classInfo->initScriptProps();
}

void ScriptSystem::linkMetaInfoToClassInfo(MetaInfo *mi)
{
	ClassInfoDict::iterator it = m_classInfoReg.begin();

	for (; it != m_classInfoReg.end(); ++it) {
		ClassInfo *ci = it->second;
		if (ci->m_metaName == mi->m_name) {
			ci->m_typeInfo = mi;
			mi->m_classInfo = ci;
		}
	}
}

void ScriptSystem::getClassList(const char *prefix, bool sort, StringSeq &result) const
{
	ClassInfoDict::const_iterator it = m_classInfoReg.begin();
	size_t prefixlen = 0;
	
	if (prefix) {
		prefixlen = strlen(prefix);
	}

	for (; it != m_classInfoReg.end(); ++it) {
		ClassInfo *ci = it->second;
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

AX_END_NAMESPACE

