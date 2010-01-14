#include "script_p.h"

AX_BEGIN_NAMESPACE

_IMPL_NATIVE_CONSTRUCTION(Vector3, Vector3);

_MEMBER_FUNCTION_IMPL(Vector3, constructor)
{
	Vector3 temp;
	Vector3 *newv = NULL;
	StackHandler sa(v);
	int nparams = sa.GetParamCount();
	switch (nparams) {
	case 1:
		temp.x = 0;
		temp.y = 0;
		temp.z = 0;
		break;
	case 2:
		if (sa.GetType(2) == OT_INSTANCE) {
			_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
			if(vec)	temp = *vec;
			else return sa.ThrowError(_SC("Vector3() invalid instance type"));
		}
		break;
	case 4:
		temp.x = sa.GetFloat(2);
		temp.y = sa.GetFloat(3);
		temp.z = sa.GetFloat(4);
		break;
	default:
		return sa.ThrowError(_SC("Vector3() wrong parameters"));
	}
	newv = new Vector3(temp);
	return construct_Vector3(g_mainVM->m_vm, newv);
}


_MEMBER_FUNCTION_IMPL(Vector3,_set)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	const SQChar *s = sa.GetString(2);
	int index = s?s[0]:sa.GetInt(2);
	switch(index) {
	case 0: case 'x': case 'r':
		return sa.Return(self->x = sa.GetFloat(3));
		break;
	case 1: case 'y': case 'g':
		return sa.Return(self->y = sa.GetFloat(3));
		break;
	case 2: case 'z': case 'b':
		return sa.Return(self->z = sa.GetFloat(3));
		break;
	}

	return SQ_ERROR;
}

_MEMBER_FUNCTION_IMPL(Vector3,_get)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	const SQChar *s = sa.GetString(2);
	if(s && (s[1] != 0))
		return SQ_ERROR;
	int index = s && (s[1] == 0)?s[0]:sa.GetInt(2);
	switch(index) {
		case 0: case 'x': case 'r': return sa.Return(self->x); break;
		case 1: case 'y': case 'g':	return sa.Return(self->y); break;
		case 2: case 'z': case 'b': return sa.Return(self->z); break;
	}
	return SQ_ERROR;
}

_MEMBER_FUNCTION_IMPL(Vector3,_nexti)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	int index = -1;
	if(sa.GetType(2) == OT_NULL) {
		index = -1;
	} else {
		const SQChar *s = sa.GetString(2);
		if (s && (s[1] != 0))
			return SQ_ERROR;
		index = s && (s[1] == 0)?s[0]:sa.GetInt(2);
	}
	switch(index) {
		case 0xFFFFFFFF: return sa.Return(_SC("x"));
		case 0: case 'x': case 'r': return sa.Return(_SC("y"));
		case 1: case 'y': case 'g': return sa.Return(_SC("z"));
		case 2: case 'z': case 'b': return sa.Return();
	}
	return sa.Return(_SC("invalid index"));
}

_MEMBER_FUNCTION_IMPL(Vector3,_cmp)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
	if((*self) == (*vec))
		return sa.Return(0);
	if((*self) < (*vec))
		return sa.Return(-1);
	return sa.Return(1);
}

_MEMBER_FUNCTION_IMPL(Vector3,_add)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
	Vector3 tv = (*self)+(*vec);
	SquirrelObject so = new_Vector3(v, tv);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Vector3,_sub)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
	Vector3 tv = (*self)-(*vec);
	SquirrelObject so = new_Vector3(v, tv);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Vector3,_mul)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	Vector3 tv = (*self)*sa.GetFloat(2);
	SquirrelObject so = new_Vector3(v, tv);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Vector3,_div)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	Vector3 tv = (*self)/sa.GetFloat(2);
	SquirrelObject so = new_Vector3(v, tv);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Vector3,DotProduct)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
	return sa.Return(*self | *vec);
}

_MEMBER_FUNCTION_IMPL(Vector3,CrossProduct)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
	Vector3 ret;
	ret = *self ^ *vec;
	SquirrelObject so = new_Vector3(v, ret);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Vector3,SquareDistance)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
	Vector3 tmp = *self - *vec;
	return sa.Return(tmp.getLengthSquared());
}

_MEMBER_FUNCTION_IMPL(Vector3,Distance)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	_CHECK_INST_PARAM(vec,2,Vector3,Vector3);
	Vector3 tmp = *self - *vec;
	return sa.Return(tmp.getLength());
}

_MEMBER_FUNCTION_IMPL(Vector3,Length)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	return sa.Return(self->getLength());
}

_MEMBER_FUNCTION_IMPL(Vector3,SquareLength)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	return sa.Return(self->getLengthSquared());
}

_MEMBER_FUNCTION_IMPL(Vector3,Normalize)
{
	_CHECK_SELF(Vector3,Vector3);
	self->normalize();
	return 0;
}

_MEMBER_FUNCTION_IMPL(Vector3,GetNormalized)
{
	StackHandler sa(v);
	_CHECK_SELF(Vector3,Vector3);
	Vector3 tmp = self->getNormalized();
	SquirrelObject so = new_Vector3(v, tmp);
	return sa.Return(so);
}


_BEGIN_CLASS(Vector3)
_MEMBER_FUNCTION(Vector3,constructor,-1,_SC(".n|xnn"))
_MEMBER_FUNCTION(Vector3,_set,3,_SC("xs|n"))
_MEMBER_FUNCTION(Vector3,_get,2,_SC("xs|n"))
_MEMBER_FUNCTION(Vector3,_add,2,_SC("xx"))
_MEMBER_FUNCTION(Vector3,_sub,2,_SC("xx"))
_MEMBER_FUNCTION(Vector3,_mul,2,_SC("xn"))
_MEMBER_FUNCTION(Vector3,_div,2,_SC("xn"))
_MEMBER_FUNCTION(Vector3,_nexti,2,_SC("x"))
_MEMBER_FUNCTION(Vector3,_cmp,2,_SC("xx"))
_MEMBER_FUNCTION(Vector3,DotProduct,2,_SC("xx"))
_MEMBER_FUNCTION(Vector3,CrossProduct,2,_SC("xx"))
_MEMBER_FUNCTION(Vector3,SquareDistance,2,_SC("xx"))
_MEMBER_FUNCTION(Vector3,Distance,2,_SC("xx"))
_MEMBER_FUNCTION(Vector3,Length,1,_SC("x"))
_MEMBER_FUNCTION(Vector3,SquareLength,1,_SC("x"))
_MEMBER_FUNCTION(Vector3,Normalize,1,_SC("x"))
_MEMBER_FUNCTION(Vector3,GetNormalized,1,_SC("x"))
_END_CLASS(Vector3)

AX_END_NAMESPACE
