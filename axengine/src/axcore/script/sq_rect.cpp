#include "script_p.h"

AX_BEGIN_NAMESPACE

_IMPL_NATIVE_CONSTRUCTION(Rect, Rect);

_MEMBER_FUNCTION_IMPL(Rect, constructor)
{
	StackHandler sa(v);
	_CHECK_SELF(Rect, Rect);
	int nparams = sa.getParamCount();
	switch (nparams) {
	case 1:
		self->set(0,0,0,0);
		break;
	case 2:
		if (sa.getType(2) == OT_INSTANCE) {
			_CHECK_INST_PARAM(vec,2,Rect,Rect);
			if (vec) *self = *vec;
			else return sa.throwError(_SC("Rect() invalid instance type"));
		}
		break;
	case 5:
		self->x = sa.getInt(2);
		self->y = sa.getInt(3);
		self->width = sa.getInt(4);
		self->height = sa.getInt(5);
		break;
	default:
		return sa.throwError(_SC("Rect() wrong parameters"));
	}
	return 1;
}

_BEGIN_CLASS(Rect)
_MEMBER_FUNCTION(Rect,constructor,-1,_SC(".n|xnnn"))
_END_CLASS(Rect)

AX_END_NAMESPACE
