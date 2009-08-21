/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_ROTATION_H
#define AX_CORE_ROTATION_H

namespace Axon {

	struct AX_API Rotate : public Vector3 {
	public:
		void fromAxis(const Matrix3& axis);
		Matrix3 toAxis() const;
	};

	inline void Rotate::fromAxis(const Matrix3& axis) {
		double theta;
		double cp;
		double sp;

		sp = axis[0][2];

		// cap off our sin value so that we don't get any NANs
		if (sp > 1.0) {
			sp = 1.0;
		} else if (sp < -1.0) {
			sp = -1.0;
		}

		theta = -asin(sp);
		cp = cos(theta);

//		const float FLT_EPSILON = 1.192092896e-07f;        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
		if (cp > 8192 * FLT_EPSILON) {
			(*this)[/*pitch*/1] = theta * 180 / AX_PI;
			(*this)[/*yaw*/2]   = atan2(axis[0][1], axis[0][0]) * 180 / AX_PI;
			(*this)[/*roll*/0]  = atan2(axis[1][2], axis[2][2]) * 180 / AX_PI;
		} else {
			(*this)[/*pitch*/1] = theta * 180 / AX_PI;
			(*this)[/*yaw*/2]   = -atan2(axis[1][0], axis[1][1]) * 180 / AX_PI;
			(*this)[/*roll*/0]  = 0;
		}
	}

	inline Matrix3 Rotate::toAxis() const {
		Matrix3 result;

		float angle;
		float sz, sx, sy, cz, cx, cy;

		angle = (*this)[/*yaw*/2] * AX_PI / 180;
		sy = sin(angle);
		cy = cos(angle);
		angle = (*this)[/*pitch*/1] * AX_PI / 180;
		sx = sin(angle);
		cx = cos(angle);
		angle = (*this)[/*roll*/0] * AX_PI / 180;
		sz = sin(angle);
		cz = cos(angle);

		result[0][0] = cx*cy;
		result[0][1] = cx*sy;
		result[0][2] = -sx;

		result[1][0] = sz*sx*cy+cz*-sy;
		result[1][1] = sz*sx*sy+cz*cy;
		result[1][2] = sz*cx;

		result[2][0] = (cz*sx*cy+-sz*-sy);
		result[2][1] = (cz*sx*sy+-sz*cy);
		result[2][2] = cz*cx;

		return result;
	}

} // namespace Axon

#endif // AX_CORE_ROTATION_H

