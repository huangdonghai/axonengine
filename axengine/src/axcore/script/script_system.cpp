#include "script_p.h"

/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#if 0
#define OBJECT_MT "__object_mt"
#define OBJECT_NAME "__object"
#endif

AX_BEGIN_NAMESPACE

sqVM *g_mainVM;
HSQUIRRELVM VM;

//--------------------------------------------------------------------------
// class ScriptSystem
//--------------------------------------------------------------------------
struct Connection {
	Object *obj;
	std::string callname;
};

typedef std::list<Connection> ConnectionSeq;
typedef Dict<Object*,Dict<std::string, ConnectionSeq>> Connections;

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
#if 0
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
#endif
	g_mainVM = new sqVM();
	_INIT_CLASS(Vector3);
	_INIT_CLASS(Color3);
	_INIT_CLASS(Point);
	_INIT_CLASS(Rect);
	_INIT_CLASS(Matrix);
	_INIT_CLASS(Object_c);

	g_mainVM->runFile("start.nut");

	Printf(_("Initialized ScriptSystem\n"));
}

void ScriptSystem::finalize()
{
	SafeDelete(g_mainVM);
#if 0
	lua_close(L);
#endif
}

#if 0
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
#endif

void ScriptSystem::executeLine( const char *text )
{
	sqObject bytecode = g_mainVM->compileBuffer(text);
	g_mainVM->runBytecode(bytecode);
}

#if 0
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
			if (Variant::kTable == variant.getTypeId()) {
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
		if (variant.getTypeId() == Variant::kString)
			result = variant.toString();
		lua_pop(L,1);
		id = lua_gettop(L);
	} else {
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
			if (var.getTypeId() == Variant::kString)
				result = var;						
		}else{
			lua_getglobal(L,objectname.c_str());
			Variant variant = xReadStack(L,-1);
			
			if (variant.getTypeId() == Variant::kTable){
				xPushString(L, membername);
				lua_gettable(L, -2);
				Variant res = xReadStack(L,-1);
				if (res.getTypeId() == Variant::kString)
					result = res.toString();
				lua_pop(L,1);
			}
			lua_pop(L,1);
			id = lua_gettop(L);
		}
	}
	return result;
}
#endif

inline std::string xRemoveScope(const std::string &str)
{
	size_t pos = str.rfind('.');
	if (pos == std::string::npos || pos >= str.length()-1) {
		return str;
	}

	return str.substr(pos+1, str.length()-pos-1);
}

inline std::string xRemoveIndex(const std::string &str)
{
	// first get index from str
	std::string::const_reverse_iterator it = str.rbegin();

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

int ScriptSystem::getNameIndex(const std::string &str) const
{
	StringIntDict::const_iterator it = m_objectNameGen.find(xRemoveIndex(str));
	if (it == m_objectNameGen.end())
		return 0;
	return it->second;
}

void ScriptSystem::updateNameIndex(const std::string &str)
{
	// first get index from str
	std::string::const_reverse_iterator it = str.rbegin();

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
	std::string name(str);

	if (!noindex) {
		index = atoi(p + 1);
		name = std::string(&str[0], p);
	}

	// check if need update
	int curindex = getNameIndex(name);
	if (index < curindex)
		return;

	m_objectNameGen[name] = index+1;
}

int ScriptSystem::nextNameIndex(const std::string &str)
{
	return m_objectNameGen[str]++;
}

std::string ScriptSystem::generateObjectName(const std::string &str)
{
	std::string noindex = xRemoveScope(xRemoveIndex(str));

	int index = nextNameIndex(noindex);

	if (!index)
		return noindex;

	std::string result;
	StringUtil::sprintf(result, "%s_%d", noindex.c_str(), index);
	return result;
}

void ScriptSystem::registerCppClass(CppClass *metainfo)
{
	m_cppClassReg[metainfo->getName()] = metainfo;

	linkCppToScript(metainfo);
}

Object *ScriptSystem::createObject(const FixedString &classname)
{
	ScriptClassDict::const_iterator cit = m_scriptClassReg.find(classname);
	if (cit != m_scriptClassReg.end()) {
		ScriptClass *ci = cit->second;

		if (!ci->m_cppClass) {
			Errorf("can't find type info");
		}

		// create object
		Object *obj = ci->m_cppClass->createObject();
		obj->initScriptClass(ci);

		return obj;
	}

	CppClassDict::const_iterator it = m_cppClassReg.find(classname);

	if (it == m_cppClassReg.end()) {
		return nullptr;
	}

	Object *obj = it->second->createObject();
	return obj;
}

Object *ScriptSystem::cloneObject(const Object *obj)
{
	Object *result = 0;
	
	if (obj->getScriptClass())
		result = createObject(obj->getScriptClass()->m_name.c_str());

	if (!result)
		result = obj->getCppClass()->createObject();

	if (!result)
		Errorf("can't create object");

	// copy properties
	result->copyPropertiesFrom(obj);

	return result;
}

#if 0
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
#endif


void ScriptSystem::registerScriptClass( const std::string &name )
{
	ScriptClassDict::iterator it = m_scriptClassReg.find(name);

	if (it != m_scriptClassReg.end())
		Errorf("Class already registered");

	ScriptClass * sqclass = new ScriptClass(name);
	m_scriptClassReg[name] = sqclass;
}

void ScriptSystem::linkCppToScript(CppClass *mi)
{
	ScriptClassDict::iterator it = m_scriptClassReg.begin();

	for (; it != m_scriptClassReg.end(); ++it) {
		ScriptClass *ci = it->second;
		if (ci->m_cppName == mi->m_name) {
			ci->m_cppClass = mi;
			mi->m_scriptClass = ci;
		}
	}
}

void ScriptSystem::getClassList(const char *prefix, bool sort, StringSeq &result) const
{
	ScriptClassDict::const_iterator it = m_scriptClassReg.begin();
	size_t prefixlen = 0;
	
	if (prefix) {
		prefixlen = strlen(prefix);
	}

	for (; it != m_scriptClassReg.end(); ++it) {
		ScriptClass *ci = it->second;
		if (prefixlen) {
			if (strncmp(prefix, ci->m_name.c_str(), prefixlen) != 0) {
				continue;
			}

			result.push_back(ci->m_name.c_str() + prefixlen);
		} else {
			result.push_back(ci->m_name.toString());
		}
	}

	if (sort) {
		std::sort(result.begin(), result.end(), std::less<std::string>());
	}
}

extern int ScriptMetacall(HSQUIRRELVM vm);

ScriptValue ScriptSystem::createMetaClosure(Member *method)
{
	int top = 0;
	sq_pushuserpointer(VM, method);
	sq_newclosure(VM, ScriptMetacall, 1);
	sq_setparamscheck(VM, 0, 0);
	sq_setnativeclosurename(VM, -1, method->getName().c_str());

	sqObject sobj;
	sobj.attachToStackObject(VM, -1);
	sq_settop(VM, top);

	return sobj;
}

ScriptClass *ScriptSystem::findScriptClass(const char *name) const
{
	ScriptClassDict::const_iterator it = m_scriptClassReg.find(name);

	return it->second;
}

CppClass * ScriptSystem::findCppClass(const char *name) const
{
	return m_cppClassReg.find(name)->second;
}

void ScriptSystem::allocThread(ScriptThread &thread)
{
	int id = sqVM::allocThread();
	thread = ScriptThread(sqVM::ms_threadPool[id], id);
}


AX_END_NAMESPACE

