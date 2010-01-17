#include "script_p.h"

AX_BEGIN_NAMESPACE

_IMPL_NATIVE_CONSTRUCTION(Point, Point);

_MEMBER_FUNCTION_IMPL(Point, constructor)
{
	Point temp;
	Point *newv = NULL;
	StackHandler sa(v);
	int nparams = sa.GetParamCount();
	switch (nparams) {
	case 1:
		temp.x = 0;
		temp.y = 0;
		break;
	case 2:
		if (sa.GetType(2) == OT_INSTANCE) {
			_CHECK_INST_PARAM(vec,2,Point,Point);
			if(vec)	temp = *vec;
			else return sa.ThrowError(_SC("Point() invalid instance type"));
		}
		break;
	case 3:
		temp.x = sa.GetInt(2);
		temp.y = sa.GetInt(3);
		break;
	default:
		return sa.ThrowError(_SC("Point() wrong parameters"));
	}
	newv = new Point(temp);
	return construct_Point(v, newv);
}

_BEGIN_CLASS(Point)
_MEMBER_FUNCTION(Point,constructor,-1,_SC(".n|xn"))
_END_CLASS(Point)

AX_END_NAMESPACE
