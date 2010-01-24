#include "script_p.h"

AX_BEGIN_NAMESPACE

int ScriptMetacall(HSQUIRRELVM v)
{
	StackHandler sa(v);
	_CHECK_SELF_OBJ();
	int nargs = sa.getParamCount();

	Member *member = (Member *)sa.getUserPointer(nargs);

	AX_ASSERT(member->isMethod());

	int argc = member->argc();
	if (nargs - 2 < argc)
		return SQ_ERROR;

	const Variant::TypeId * argTypes = member->getArgsType();
	Variant values[Member::MaxArgs];
	const void *argv[Member::MaxArgs];

	for (int i = 0; i < argc; i++) {
		sa.getVariant(i+2, values[i]);

		if (values[i].getTypeId() != argTypes[i]) {
			bool castSuccess = values[i].castSelf(argTypes[i]);
			if (!castSuccess)
				return SQ_ERROR;
		}

		argv[i] = values[i].getPointer();
	}

	Variant ret(member->getReturnType());
	int nret = member->invoke(self, ret.getPointer(), argv);

	if (nret = 0) return 0;

	return sa.Return(ConstRef(ret.getTypeId(), ret.getPointer()));
}

_IMPL_NATIVE_CONSTRUCTION(Object_c, ObjectStar);

_MEMBER_FUNCTION_IMPL(Object_c, constructor)
{
	StackHandler sa(v);
	_CHECK_SELF_OBJ();
	self = 0;
	return 1;
}

_MEMBER_FUNCTION_IMPL(Object_c, _get)
{
	StackHandler sa(v);
	_CHECK_SELF_OBJ();
	const SQChar *s = sa.getString(2);
	CppClass *metaInfo = self->getCppClass();
	Member *member = metaInfo->findMember(s);

	if (!member) return SQ_ERROR;

	if (member->isProperty()) {
		Variant prop;

		bool success = member->getProperty(self, prop);
		if (!success)
			return SQ_ERROR;

		return sa.Return(ConstRef(prop.getTypeId(), prop.getPointer()));
	}

	return sa.Return(member->getScriptClousure().getSqObject());
	return SQ_ERROR;
//	return sa.Return(member->m_scriptClosure);
}

_MEMBER_FUNCTION_IMPL(Object_c, _set)
{
	StackHandler sa(v);
	_CHECK_SELF_OBJ();
	const SQChar *s = sa.getString(2);
	CppClass *metaInfo = self->getCppClass();
	Member *member = metaInfo->findMember(s);

	if (!member || !member->isProperty()) return SQ_ERROR;

	Variant::TypeId propType = member->getPropType();
	Variant value;
	sa.getVariant(3, value);

	if (!value.castSelf(propType))
		return SQ_ERROR;

	bool success = member->setProperty(self, ConstRef(value.getTypeId(), value.getPointer()));

	return success ? 0 : SQ_ERROR;
}

_BEGIN_CLASS(Object_c)
_MEMBER_FUNCTION(Object_c,constructor, -1, _SC("."))
_MEMBER_FUNCTION(Object_c,_set, 3, _SC("xs|n"))
_MEMBER_FUNCTION(Object_c,_get, 2, _SC("xs|n"))
_END_CLASS(Object_c)


void sqVM::createObjectClosure()
{
	sqVM::ms_setClosure = sqVM::createClosure("_get", __Object_c__set, 3, _SC("xs|n"));
	sqVM::ms_getClosure = sqVM::createClosure("_set", __Object_c__get, 2, _SC("xs|n"));
}

AX_END_NAMESPACE
