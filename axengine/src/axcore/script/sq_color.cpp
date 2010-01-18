#include "script_p.h"

#if 1
AX_BEGIN_NAMESPACE

_IMPL_NATIVE_CONSTRUCTION(Color3, Color3);

_MEMBER_FUNCTION_IMPL(Color3, constructor)
{
	Color3 temp;
	Color3 *newv = NULL;
	StackHandler sa(v);
	int nparams = sa.getParamCount();
	switch (nparams) {
	case 1:
		temp.r = 0;
		temp.g = 0;
		temp.b = 0;
		break;
	case 2:
		if (sa.getType(2) == OT_INSTANCE) {
			_CHECK_INST_PARAM(vec,2,Color3,Color3);
			if(vec)	temp = *vec;
			else return sa.throwError(_SC("Color3() invalid instance type"));
		}
		break;
	case 4:
		temp.r = sa.getFloat(2);
		temp.g = sa.getFloat(3);
		temp.b = sa.getFloat(4);
		break;
	default:
		return sa.throwError(_SC("Color3() wrong parameters"));
	}
	newv = new Color3(temp);
	return construct_Color3(g_mainVM->m_vm, newv);
}


_MEMBER_FUNCTION_IMPL(Color3,_set)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	const SQChar *s = sa.getString(2);
	int index = s?s[0]:sa.getInt(2);
	switch(index) {
	case 0: case 'r':
		return sa.Return(self->r = sa.getFloat(3));
		break;
	case 1: case 'g':
		return sa.Return(self->g = sa.getFloat(3));
		break;
	case 2: case 'b':
		return sa.Return(self->b = sa.getFloat(3));
		break;
	}

	return SQ_ERROR;
}

_MEMBER_FUNCTION_IMPL(Color3,_get)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	const SQChar *s = sa.getString(2);
	if(s && (s[1] != 0))
		return SQ_ERROR;
	int index = s && (s[1] == 0)?s[0]:sa.getInt(2);
	switch(index) {
		case 0: case 'r': return sa.Return(self->r); break;
		case 1: case 'g': return sa.Return(self->g); break;
		case 2: case 'b': return sa.Return(self->b); break;
	}
	return SQ_ERROR;
}

_MEMBER_FUNCTION_IMPL(Color3,_nexti)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	int index = -1;
	if(sa.getType(2) == OT_NULL) {
		index = -1;
	} else {
		const SQChar *s = sa.getString(2);
		if (s && (s[1] != 0))
			return SQ_ERROR;
		index = s && (s[1] == 0)?s[0]:sa.getInt(2);
	}
	switch(index) {
		case 0xFFFFFFFF: return sa.Return(_SC("x"));
		case 0: case 'x': case 'r': return sa.Return(_SC("y"));
		case 1: case 'y': case 'g': return sa.Return(_SC("z"));
		case 2: case 'z': case 'b': return sa.Return();
	}
	return sa.Return(_SC("invalid index"));
}

_MEMBER_FUNCTION_IMPL(Color3,_cmp)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	_CHECK_INST_PARAM(vec,2,Color3,Color3);
	if ((*self) == (*vec))
		return sa.Return(0);
	if((*self) < (*vec))
		return sa.Return(-1);
	return sa.Return(1);
}

_MEMBER_FUNCTION_IMPL(Color3,_add)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	_CHECK_INST_PARAM(vec,2,Color3,Color3);
	Color3 tv = (*self)+(*vec);
	SquirrelObject so = new_Color3(v, tv);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Color3,_sub)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	_CHECK_INST_PARAM(vec,2,Color3,Color3);
	Color3 tv = (*self)-(*vec);
	SquirrelObject so = new_Color3(v, tv);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Color3,_mul)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	Color3 tv = (*self)*sa.getFloat(2);
	SquirrelObject so = new_Color3(v, tv);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Color3,_div)
{
	StackHandler sa(v);
	_CHECK_SELF(Color3,Color3);
	Color3 tv = (*self)/sa.getFloat(2);
	SquirrelObject so = new_Color3(v, tv);
	return sa.Return(so);
}

_BEGIN_CLASS(Color3)
_MEMBER_FUNCTION(Color3,constructor,-1,_SC(".n|xnn"))
_MEMBER_FUNCTION(Color3,_set,3,_SC("xs|n"))
_MEMBER_FUNCTION(Color3,_get,2,_SC("xs|n"))
_MEMBER_FUNCTION(Color3,_add,2,_SC("xx"))
_MEMBER_FUNCTION(Color3,_sub,2,_SC("xx"))
_MEMBER_FUNCTION(Color3,_mul,2,_SC("xn"))
_MEMBER_FUNCTION(Color3,_div,2,_SC("xn"))
_MEMBER_FUNCTION(Color3,_nexti,2,_SC("x"))
_MEMBER_FUNCTION(Color3,_cmp,2,_SC("xx"))
_END_CLASS(Color3)

AX_END_NAMESPACE

#endif