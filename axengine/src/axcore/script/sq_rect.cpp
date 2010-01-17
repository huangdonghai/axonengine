#include "script_p.h"

AX_BEGIN_NAMESPACE

_IMPL_NATIVE_CONSTRUCTION(Rect, Rect);

_MEMBER_FUNCTION_IMPL(Rect, constructor)
{
	Rect temp;
	Rect *newv = NULL;
	StackHandler sa(v);
	int nparams = sa.GetParamCount();
	switch (nparams) {
	case 1:
		temp.x = 0;
		temp.y = 0;
		temp.width = 0;
		temp.height = 0;
		break;
	case 2:
		if (sa.GetType(2) == OT_INSTANCE) {
			_CHECK_INST_PARAM(vec,2,Rect,Rect);
			if(vec)	temp = *vec;
			else return sa.ThrowError(_SC("Rect() invalid instance type"));
		}
		break;
	case 5:
		temp.x = sa.GetInt(2);
		temp.y = sa.GetInt(3);
		temp.width = sa.GetInt(4);
		temp.height = sa.GetInt(5);
		break;
	default:
		return sa.ThrowError(_SC("Rect() wrong parameters"));
	}
	newv = new Rect(temp);
	return construct_Rect(v, newv);
}

_BEGIN_CLASS(Rect)
_MEMBER_FUNCTION(Rect,constructor,-1,_SC(".n|xnnn"))
_END_CLASS(Rect)

AX_END_NAMESPACE
