/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

const BoundingBox BoundingBox::EmptyBox(99999,99999,99999,-99999,-99999,-99999);
const BoundingBox BoundingBox::UnitBox(-1,-1,-1,1,1,1);
const BoundingBox BoundingBox::LargestBox(-99999,-99999,-99999,99999,99999,99999);



// some function is recursively referanced, so move to here
BoundingBox BoundingBox::getTransformed(const AffineMat &matrix) const {
	int i;
	Vector3 center, extents, rotatedExtents;
	BoundingBox out;

	center = getCenter();
	extents = max - center;

	const Matrix3 &axis = matrix.axis;
	const Vector3 &origin = matrix.origin;

	for (i = 0; i < 3; i++) {
		rotatedExtents[i] = fabsf(extents[0] * axis[0][i]) +
			fabsf(extents[1] * axis[1][i]) +
			fabsf(extents[2] * axis[2][i]);
	}

	center = origin + axis * center;
	out.min = center - rotatedExtents;
	out.max = center + rotatedExtents;

	return out;
}


AX_END_NAMESPACE

