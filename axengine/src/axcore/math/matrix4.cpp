/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

	const Matrix4 Matrix4::Identity(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);

	AffineMat Matrix4::getAffineMat() const {
		AffineMat result;

		extract3x3Matrix(result.axis);
		result.origin = m[3].xyz();

		return result;
	}

AX_END_NAMESPACE
