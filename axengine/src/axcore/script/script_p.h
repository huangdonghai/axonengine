#include "../private.h"

#define OBJECT_MT "__object_mt"
#define OBJECT_NAME "__object"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>	
#include <sqstdstring.h>
#include <sqstdaux.h>

#include "sq_binding.h"
#include "sq_helper.h"

AX_BEGIN_NAMESPACE

_DECL_CLASS(Vector3);
_DECL_CLASS(Color3);
_DECL_CLASS(Point);
_DECL_CLASS(Rect);
_DECL_CLASS(Matrix)
_DECL_CLASS(Object_c);
_DECL_NATIVE_CONSTRUCTION(Vector3, Vector3);
_DECL_NATIVE_CONSTRUCTION(Color3, Color3);
_DECL_NATIVE_CONSTRUCTION(Point, Point);
_DECL_NATIVE_CONSTRUCTION(Rect, Rect);
_DECL_NATIVE_CONSTRUCTION(Matrix, Matrix);

extern lua_State *L;
Variant xReadStack(lua_State *L, int index);
void xPushStack(lua_State *L, const Variant &val);
bool xGetGlobalScoped(lua_State *L, const char *name);
int xReport(lua_State *L, int status, const char *progname);
void xPushString(lua_State *L, const String &s);
int xPcall(lua_State *L, int numarg, int numresult);
bool xSetScoped(lua_State *L, const char *name, const Variant &val);
bool xGetScoped(lua_State *L, const char *name);

extern SquirrelVM *g_mainVM;
extern HSQUIRRELVM VM;

AX_END_NAMESPACE
