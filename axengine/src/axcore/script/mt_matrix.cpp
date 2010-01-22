#include "script_p.h"

AX_BEGIN_NAMESPACE

_IMPL_NATIVE_CONSTRUCTION(Matrix, Matrix);

_MEMBER_FUNCTION_IMPL(Matrix, constructor)
{
	StackHandler sa(v);
	_CHECK_SELF(Matrix,Matrix);
	switch (sa.getParamCount()) {
		case 1:
			self->setIdentity();
			break;
		case 2:
			if (sa.getType(2) == OT_INSTANCE) {
				_CHECK_INST_PARAM(mat,2,Matrix,Matrix);
				if (mat)
					*self = *mat;
				else
					return sa.throwError(_SC("Matrix() invalid instance type"));
			} else {
				sqObject arr = sa.getObjectHandle(2);
				if (arr.len() != Matrix::NumFloat) {
					return sa.throwError(_SC("Matrix(array) need a 12 elements array"));
				}
				float *fp = self->w_ptr();
				sqObject idx, val;
				if(arr.beginIteration()) {
					while(arr.next(idx,val)) {
						fp[idx.toInteger()] = val.toFloat();
					}
					arr.endIteration();
				}
			}
			break;
		default:
			return sa.throwError(_SC("Matrix() wrong number of parameters"));
			break;
	}
	return 1;
}

_MEMBER_FUNCTION_IMPL(Matrix,_set)
{
	StackHandler sa(v);
	_CHECK_SELF(Matrix,Matrix);
	int index = sa.getInt(2);
	if (index < 0 && index >= Matrix::NumFloat)
		return SQ_ERROR;
	self->w_ptr()[index] = sa.getFloat(2);
	return SQ_OK;
}

_MEMBER_FUNCTION_IMPL(Matrix,_get)
{
	StackHandler sa(v);
	_CHECK_SELF(Matrix,Matrix);
	int index = sa.getInt(2);
	if (index < 0 && index >= Matrix::NumFloat)
		return SQ_ERROR;
	return sa.Return(self->w_ptr()[index]);
}

#if 0
_MEMBER_FUNCTION_IMPL(Matrix,_add)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(mat,2,AffineMat,Matrix);
	AffineMat tm = (*self)+(*mat);
	sqObject so = new_Matrix(tm);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Matrix,_sub)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(mat,2,AffineMat,Matrix);
	AffineMat tm = (*self)-(*mat);
	sqObject so = new_Matrix(tm);
	return sa.Return(so);
}
#endif

_MEMBER_FUNCTION_IMPL(Matrix,_mul)
{
	StackHandler sa(v);
	_CHECK_SELF(Matrix,Matrix);
	int t = sa.getType(2);
	if (t == OT_INSTANCE) {
		_CHECK_INST_PARAM(mat,2,Matrix,Matrix);
		Matrix tm = (*self)*(*mat);
        sqObject so = new_Matrix(v, tm);
        return sa.Return(so);
	}
#if 0
	AffineMat tm = (*self)*sa.GetFloat(2);
    sqObject so = new_Matrix(tm);
    return sa.Return(so);
#else
	return SQ_ERROR;
#endif
}

#if 0
_MEMBER_FUNCTION_IMPL(Matrix,_div)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	AffineMat tm = (*self)/sa.GetFloat(2);
    sqObject so = new_Matrix(tm);
    return sa.Return(so);
}
#endif
_MEMBER_FUNCTION_IMPL(Matrix,setIdentity)
{
	_CHECK_SELF(Matrix,Matrix);
	self->setIdentity();
	return 0;
}

#if 0
_MEMBER_FUNCTION_IMPL(Matrix,CreateRotationAxis)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	AffineMatRotationAxis(self,vec,sa.GetFloat(3));
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,CreateRotationAngles)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	AffineMatRotationAxis(self,vec,sa.GetFloat(3));
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,CreateScalingMatrix)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	AffineMatScaling(self,sa.GetFloat(2),sa.GetFloat(3),sa.GetFloat(4));
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,CreateTranslationMatrix)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	AffineMatTranslation(self,sa.GetFloat(2),sa.GetFloat(3),sa.GetFloat(4));
	return 0;
}
_MEMBER_FUNCTION_IMPL(Matrix,CreateLookAtMatrix)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(eye,2,D3DXVECTOR3,Vector3);
	_CHECK_INST_PARAM(at,3,D3DXVECTOR3,Vector3);
	_CHECK_INST_PARAM(up,4,D3DXVECTOR3,Vector3);
	AffineMatLookAtLH(self,eye,at,up);
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,CreatePerspectiveFovMatrix)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	AffineMatPerspectiveFovLH(self,sa.GetFloat(2),sa.GetFloat(3),sa.GetFloat(4),sa.GetFloat(5));
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,RotateAngles)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	AffineMatRotationYawPitchRoll(self,vec->y,vec->x,vec->z);
	return 0;
}


_MEMBER_FUNCTION_IMPL(Matrix,RotateAxis)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	AffineMatRotationAxis(self,vec,sa.GetFloat(3));
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,Translate)
{
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	AffineMat temp;
	AffineMatTranslation(&temp,vec->x,vec->y,vec->z);
	AffineMatMultiply(self,&temp,self);
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,Scale)
{
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	AffineMat temp;
	AffineMatScaling(&temp,vec->x,vec->y,vec->z);
	AffineMatMultiply(self,&temp,self);
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,GetInverse)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	AffineMat temp;
	AffineMatInverse(&temp,NULL,self);
	sqObject so = new_Matrix(temp);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Matrix,GetTransposed)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	AffineMat temp;
	AffineMatTranspose(&temp,self);
	sqObject so = new_Matrix(temp);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Matrix,GetInverseTransposed)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	AffineMat temp;
	AffineMatInverse(&temp,NULL,self);
	AffineMatTranspose(&temp,&temp);
	sqObject so = new_Matrix(temp);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Matrix,Invert)
{
	_CHECK_SELF(AffineMat,Matrix);
	AffineMatInverse(self,NULL,self);
	return 0;
}

_MEMBER_FUNCTION_IMPL(Matrix,TransformCoord)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	D3DXVECTOR3 ret;
	D3DXVec3TransformCoord(&ret,vec,self);
	sqObject so = new_Vector3(ret);
	return sa.Return(so);
}

_MEMBER_FUNCTION_IMPL(Matrix,TransformNormal)
{
	StackHandler sa(v);
	_CHECK_SELF(AffineMat,Matrix);
	_CHECK_INST_PARAM(vec,2,D3DXVECTOR3,Vector3);
	D3DXVECTOR3 ret;
	D3DXVec3TransformNormal(&ret,vec,self);
	sqObject so = new_Vector3(ret);
	return sa.Return(so);
}
#endif

_BEGIN_CLASS(Matrix)
_MEMBER_FUNCTION(Matrix,constructor,-1,_SC(".a|x"))
_MEMBER_FUNCTION(Matrix,_set,2,_SC("xn"))
_MEMBER_FUNCTION(Matrix,_get,2,_SC("xn"))
#if 0
_MEMBER_FUNCTION(Matrix,_add,2,_SC("xx"))
_MEMBER_FUNCTION(Matrix,_sub,2,_SC("xx"))
#endif
_MEMBER_FUNCTION(Matrix,_mul,2,_SC("xx|n"))
#if 0
_MEMBER_FUNCTION(Matrix,_div,2,_SC("xn"))
#endif
_MEMBER_FUNCTION(Matrix,setIdentity,1,_SC("x"))
#if 0
_MEMBER_FUNCTION(Matrix,CreateRotationAxis,3,_SC("xxn"))
_MEMBER_FUNCTION(Matrix,CreateRotationAngles,2,_SC("xx"))
_MEMBER_FUNCTION(Matrix,CreateScalingMatrix,4,_SC("xnnn"))
_MEMBER_FUNCTION(Matrix,CreateTranslationMatrix,4,_SC("xnnn"))
_MEMBER_FUNCTION(Matrix,CreateLookAtMatrix,4,_SC("xxxx"))
_MEMBER_FUNCTION(Matrix,CreatePerspectiveFovMatrix,5,_SC("xnnnn"))
_MEMBER_FUNCTION(Matrix,RotateAngles,2,_SC("xx"))
_MEMBER_FUNCTION(Matrix,RotateAxis,3,_SC("xxn"))
_MEMBER_FUNCTION(Matrix,Translate,2,_SC("xx"))
_MEMBER_FUNCTION(Matrix,Scale,2,_SC("xx"))
_MEMBER_FUNCTION(Matrix,GetInverse,1,_SC("x"))
_MEMBER_FUNCTION(Matrix,GetTransposed,1,_SC("x"))
_MEMBER_FUNCTION(Matrix,GetInverseTransposed,1,_SC("x"))
_MEMBER_FUNCTION(Matrix,Invert,1,_SC("x"))
_MEMBER_FUNCTION(Matrix,TransformCoord,2,_SC("xx"))
_MEMBER_FUNCTION(Matrix,TransformNormal,2,_SC("xx"))
#endif
_END_CLASS(Matrix)

AX_END_NAMESPACE
