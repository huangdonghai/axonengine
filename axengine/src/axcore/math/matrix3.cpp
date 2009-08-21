/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon {

	const Matrix3 Matrix3::Identity(1,0,0,0,1,0,0,0,1);

	void Matrix3::fromAngles(const Angles& angles) {
		angles.toVectors(&m[0], &m[1], &m[2]);
	}

	Angles Matrix3::toAngles() const {
		Angles angles;
		double theta;
		double cp;
		float sp;

		sp = m[ 0 ][ 2 ];

		// cap off our sin value so that we don't get any NANs
		if (sp > 1.0f) {
			sp = 1.0f;
		} else if (sp < -1.0f) {
			sp = -1.0f;
		}

		theta = -asin(sp);
		cp = cos(theta);

		if (cp > 8192.0f * FLT_EPSILON) {
			angles.pitch = Math::r2d(theta);
			angles.yaw = Math::r2d(atan2(m[ 0 ][ 1 ], m[ 0 ][ 0 ]));
			angles.roll = Math::r2d(atan2(m[ 1 ][ 2 ], m[ 2 ][ 2 ]));
		} else {
			angles.pitch = Math::r2d(theta);
			angles.yaw = Math::r2d(-atan2(m[ 1 ][ 0 ], m[ 1 ][ 1 ]));
			angles.roll = 0;
		}

		return angles;
	}

	void Matrix3::toAnglesScale(Angles& angles, float& scale) const {
		Matrix3 temp = *this;
		scale = temp.removeScale();
		angles = temp.toAngles();
	}

	Matrix3 Matrix3::getIdentity() {
		static Matrix3 result(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		);

		return result;
	}



	String Matrix3::toString() const {
		String result;

		StringUtil::sprintf(result, "%f %f %f %f %f %f %f %f %f"
			, m[0][0], m[0][1], m[0][2]
			, m[1][0], m[1][1], m[1][2]
			, m[2][0], m[2][1], m[2][2]);

		return result;
	}

	void Matrix3::fromString(const char* str) {}

}