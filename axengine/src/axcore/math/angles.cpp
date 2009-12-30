/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

	void Angles::toVectors(Vector3* forward, Vector3* left, Vector3* up) const
	{
		float angle;
		float sr, sp, sy, cr, cp, cy;
		// static to help MS compiler fp bugs

		angle = Math::d2r(yaw);
		sy = sin(angle);
		cy = cos(angle);
		angle = Math::d2r(pitch);
		sp = sin(angle);
		cp = cos(angle);
		angle = Math::d2r(roll);
		sr = sin(angle);
		cr = cos(angle);

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;

		if (left) {
			left->x = sr * sp * cy + cr * -sy;
			left->y = sr * sp * sy + cr * cy;
			left->z = sr * cp;
		}

		if (up) {
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	Vector3 Angles::toForward(void) const
	{
		float sp, sy, cp, cy;

		float angle = yaw * AX_D2R;
		sy = sin(angle);
		cy = cos(angle);
		angle = pitch * AX_D2R;
		sp = sin(angle);
		cp = cos(angle);

		return Vector3(cp * cy, cp * sy, -sp);
	}

	Matrix3 Angles::toMatrix3(void) const
	{
		Matrix3 axis;
		toVectors(&axis[0], &axis[1], &axis[2]);
		return axis;
	}

	Angles& Angles::normalize360(void)
	{
		for (int i = 0; i < 3; i++) {
			if (((*this)[i] >= 360.0f) || ((*this)[i] < 0.0f)) {
				(*this)[i] -= floor((*this)[i] / 360.0f) * 360.0f;

				if ((*this)[i] >= 360.0f) {
					(*this)[i] -= 360.0f;
				}
				if ((*this)[i] < 0.0f) {
					(*this)[i] += 360.0f;
				}
			}
		}

		return *this;
	}

	Angles& Angles::normalize180(void)
	{
		normalize360();

		if (pitch > 180.0f) {
			pitch -= 360.0f;
		}

		if (yaw > 180.0f) {
			yaw -= 360.0f;
		}

		if (roll > 180.0f) {
			roll -= 360.0f;
		}
		return *this;
	}

	Angles& Angles::fromShort(short rhs[3])
	{
		for (int i = 0; i < 3; i++) {
			(*this)[i] = rhs[i] * 360.0f / 65536.0f;
		}

		return *this;
	}

	Angles Angles::trShort(short rhs[3])
	{
		Angles result;
		result.fromShort(rhs);
		return result;
	}


AX_END_NAMESPACE

