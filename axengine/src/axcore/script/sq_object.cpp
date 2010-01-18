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

#define STACK_RET(name, id)

_IMPL_NATIVE_CONSTRUCTION(Object_c, ObjectStar);

_MEMBER_FUNCTION_IMPL(Object_c, constructor)
{
	StackHandler sa(v);
	_CHECK_SELF(ObjectStar, Object_c);
	self = 0;
	return 1;
}

_MEMBER_FUNCTION_IMPL(Object_c, _get)
{
	StackHandler sa(v);
	_CHECK_SELF(ObjectStar, Object_c);
	ObjectStar obj = *self;
	const SQChar *s = sa.getString(2);
	MetaInfo *metaInfo = obj->getMetaInfo();
	Member *member = metaInfo->findMember(s);

	if (!member) return SQ_ERROR;

	if (member->isProperty()) {
		Variant::Type propType = member->getPropType();
		ReturnArgument retarg(propType, Alloca(Variant::getTypeSize(propType)));

		bool success = member->getProperty(obj, retarg.data);
		if (!success)
			return SQ_ERROR;

		push(v, propType, retarg.data);
		return 1;
	}

	return 0;
//	return sa.Return(member->m_scriptClosure);
}

_MEMBER_FUNCTION_IMPL(Object_c, _set)
{
	StackHandler sa(v);
	_CHECK_SELF(ObjectStar, Object_c);
	ObjectStar obj = *self;
	const SQChar *s = sa.getString(2);
	MetaInfo *metaInfo = obj->getMetaInfo();
	Member *member = metaInfo->findMember(s);

	if (!member || !member->isProperty()) return SQ_ERROR;

	Variant::Type propType = member->getPropType();
	ReturnArgument retarg(propType, Alloca(Variant::getTypeSize(propType)));

	bool success = member->getProperty(obj, retarg.data);
	if (!success)
		return SQ_ERROR;

	push(v, propType, retarg.data);
	return 1;
}

AX_END_NAMESPACE
