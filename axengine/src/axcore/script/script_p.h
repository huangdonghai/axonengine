#include "../private.h"

#define OBJECT_MT "__object_mt"
#define OBJECT_NAME "__object"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

AX_BEGIN_NAMESPACE

extern lua_State *L;
Variant xReadStack(lua_State *L, int index);
void xPushStack(lua_State *L, const Variant &val);
bool xGetGlobalScoped(lua_State *L, const char *name);
int xReport(lua_State *L, int status, const char *progname);
void xPushString(lua_State *L, const String &s);
int xPcall(lua_State *L, int numarg, int numresult);
bool xSetScoped(lua_State *L, const char *name, const Variant &val);
bool xGetScoped(lua_State *L, const char *name);

AX_END_NAMESPACE
