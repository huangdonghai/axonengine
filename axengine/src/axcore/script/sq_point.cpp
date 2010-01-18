#include "script_p.h"

AX_BEGIN_NAMESPACE

_IMPL_NATIVE_CONSTRUCTION(Point, Point);

_MEMBER_FUNCTION_IMPL(Point, constructor)
{
	StackHandler sa(v);
	_CHECK_SELF(Point, Point);

	int nparams = sa.getParamCount();
	switch (nparams) {
	case 1:
		self->set(0, 0);
		break;
	case 2:
		if (sa.getType(2) == OT_INSTANCE) {
			_CHECK_INST_PARAM(vec,2,Point,Point);
			if(vec)	*self = *vec;
			else return sa.throwError(_SC("Point() invalid instance type"));
		}
		break;
	case 3:
		self->x = sa.getInt(2);
		self->y = sa.getInt(3);
		break;
	default:
		return sa.throwError(_SC("Point() wrong parameters"));
	}
	return 1;
}

_BEGIN_CLASS(Point)
_MEMBER_FUNCTION(Point,constructor,-1,_SC(".n|xn"))
_END_CLASS(Point)

AX_END_NAMESPACE
