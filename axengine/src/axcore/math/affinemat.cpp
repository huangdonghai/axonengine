/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon {

	const AffineMat AffineMat::Identity(1,0,0,0,1,0,0,0,1,0,0,0);


	String AffineMat::toString() const {
		String result;

		StringUtil::sprintf(result, "%f %f %f %f %f %f %f %f %f %f %f %f"
			, axis[0][0], axis[0][1], axis[0][2]
			, axis[1][0], axis[1][1], axis[1][2]
			, axis[2][0], axis[2][1], axis[2][2]
			, origin[0], origin[1], origin[2]);

		return result;
	}

	bool AffineMat::fromString(const char* str) {
		int v = sscanf(str, "%f %f %f %f %f %f %f %f %f %f %f %f"
			, &axis[0][0], &axis[0][1], &axis[0][2]
			, &axis[1][0], &axis[1][1], &axis[1][2]
			, &axis[2][0], &axis[2][1], &axis[2][2]
			, &origin[0], &origin[1], &origin[2]);

		return(v == 12);
	}

	void AffineMat::setInverse()
	{
		float m10 = axis[1][0], m11 = axis[1][1], m12 = axis[1][2];
		float m20 = axis[2][0], m21 = axis[2][1], m22 = axis[2][2];

		float t00 = m22 * m11 - m21 * m12;
		float t10 = m20 * m12 - m22 * m10;
		float t20 = m21 * m10 - m20 * m11;

		float m00 = axis[0][0], m01 = axis[0][1], m02 = axis[0][2];

		float invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

		t00 *= invDet; t10 *= invDet; t20 *= invDet;

		m00 *= invDet; m01 *= invDet; m02 *= invDet;

		float r00 = t00;
		float r01 = m02 * m21 - m01 * m22;
		float r02 = m01 * m12 - m02 * m11;

		float r10 = t10;
		float r11 = m00 * m22 - m02 * m20;
		float r12 = m02 * m10 - m00 * m12;

		float r20 = t20;
		float r21 = m01 * m20 - m00 * m21;
		float r22 = m00 * m11 - m01 * m10;

		float m03 = origin.x, m13 = origin.y, m23 = origin.z;

		float r03 = - (r00 * m03 + r01 * m13 + r02 * m23);
		float r13 = - (r10 * m03 + r11 * m13 + r12 * m23);
		float r23 = - (r20 * m03 + r21 * m13 + r22 * m23);

		axis = Matrix3(r00,r01,r02,r10,r11,r12,r20,r21,r22);
		origin = Vector3(r03,r13,r23);
	}

}