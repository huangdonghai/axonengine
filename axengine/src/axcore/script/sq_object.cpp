#include "script_p.h"

AX_BEGIN_NAMESPACE

// because squirrel don't support upvalues link lua, so we need register meta
// method to class
int axCallMetamethod(HSQUIRRELVM v)
{
	StackHandler sa(v);
	return sa.Return(0);
}

extern int ScriptMetacall(void *vm, Member *member)
{
	HSQUIRRELVM v = static_cast<HSQUIRRELVM>(vm);

	return 0;
}

_MEMBER_FUNCTION_IMPL(Object, _get)
{
	StackHandler sa(v);
	return sa.Return(0);
}

_MEMBER_FUNCTION_IMPL(Object, _set)
{
	StackHandler sa(v);
	return sa.Return(0);
}

AX_END_NAMESPACE
