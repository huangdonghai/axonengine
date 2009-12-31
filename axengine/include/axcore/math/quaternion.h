/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_QUATERNION_H
#define AX_CORE_QUATERNION_H

AX_BEGIN_NAMESPACE

	struct CompactQuat;

	struct AX_API Quaternion {
		float x,y,z,w;

		inline Quaternion() {}
		inline Quaternion(const Quaternion &other) {
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
		}
		inline Quaternion(float ix, float iy, float iz, float iw)
			: x(ix), y(iy), z(iz), w(iw) {}

		inline Quaternion(const Vector3 &angles) {
			fromEulerAngles(angles);
		}

		inline ~Quaternion() {}

		inline Quaternion operator=(const Quaternion &other) {
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
			return *this;
		}

		inline Quaternion operator+(const Quaternion &other) const {
			return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
		}

		inline Quaternion operator*(float s) const {
			return Quaternion(x * s, y * s, z * s, w * s);
		}

		inline Quaternion operator*(const Quaternion &a) const {
			return Quaternion(	w*a.x + x*a.w + y*a.z - z*a.y,
				w*a.y + y*a.w + z*a.x - x*a.z,
				w*a.z + z*a.w + x*a.y - y*a.x,
				w*a.w - x*a.x - y*a.y - z*a.z);
		}

		inline Quaternion operator-() const {
			return Quaternion(-x,-y,-z,-w);
		}

		inline Quaternion getInverse() const { return Quaternion(-x, -y, -z, w); }

		inline Vector3 operator*(const Vector3 &a) const {
#if 0
			Quaternion ret = this->GetInverse() * Quaternion(a.x, a.y, a.z, 0.0f) * (*this);
			return Vector3(ret.x, ret.y, ret.z);
#elif 0
			float xx = x*x;
			float yy = y*y;
			float zz = z*z;
			float ww = w*w;

			float xw2 = x*w*2.0f;
			float xy2 = x*y*2.0f;
			float xz2 = x*z*2.0f;
			float yw2 = y*w*2.0f;
			float yz2 = y*z*2.0f;
			float zw2 = z*w*2.0f;

			return Vector3(
				(xx-zz+ww-yy)*a.x + (xy2 + zw2)*a.y + (xz2 - yw2)*a.z,
				(xy2 - zw2)*a.x + (yy+ww-xx-zz)*a.y + (yz2 + xw2)*a.z,
				(xz2 + yw2)*a.x + (yz2 - xw2)*a.y + (ww-yy-xx+zz)*a.z
				);
#else
			Vector3 UV, UUV;
			Vector3 QVec(x, y, z);
			UV = QVec ^ a;
			UUV = QVec ^ UV;
			UV *= (2.0f * w);
			UUV *= 2.0f;
			return a + UV + UUV;
#endif
		}

		inline double dot(const Quaternion &other) const {
			return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
		}

		inline float operator[](int index) const {
			return *(&x+index);
		}

		inline float &operator[](int index) {
			return *(&x+index);
		}

		inline void fromEulerAngles(const Vector3 &angles) {
			float angle;
			float sr, sp, sy, cr, cp, cy;

			// FIXME: rescale the inputs to 1/2 angle
			angle = angles[2] * AX_D2R * 0.5f;
			sy = sin(angle);
			cy = cos(angle);
			angle = angles[1] * AX_D2R * 0.5f;
			sp = sin(angle);
			cp = cos(angle);
			angle = angles[0] * AX_D2R * 0.5f;
			sr = sin(angle);
			cr = cos(angle);

			x = sr*cp*cy-cr*sp*sy; // X
			y = cr*sp*cy+sr*cp*sy; // Y
			z = cr*cp*sy-sr*sp*cy; // Z
			w = cr*cp*cy+sr*sp*sy; // W
		}

		inline void fromCompact(const Vector3 &compact) {
			x = compact.x;
			y = compact.y;
			z = compact.z;
			computeW();
		}

		inline void fromCompact(const CompactQuat &cquat);

		/** Gets the shortest arc quaternion to rotate this vector to the destination
		vector.
		@remarks
		If you call this with a dest vector that is close to the inverse
		of this vector, we will rotate 180 Quaternions around the 'fallbackAxis'
		(if specified, or a generated axis if not) since in this case
		ANY axis of rotation is valid.
		*/
		inline void fromVectorToVector(const Vector3 &from,const Vector3 &dest,const Vector3 &fallbackAxis = Vector3(0.0f,0.0f,0.0f))
		{
			// Copy, since cannot modify local
			Vector3 v0 = from;
			Vector3 v1 = dest;
			v0.normalize();
			v1.normalize();

			float d = v0 | v1;
			// If dot == 1, vectors are the same
			if (d >= 1.0f)
			{
				*this = Quaternion(1.0f,0.0f,0.0f,0.0f);
			}
			if (d < (1e-6f - 1.0f))
			{
				if (fallbackAxis != Vector3(0.0f,0.0f,0.0f))
				{
					// rotate 180 Quaternions about the fallback axis
					fromAxisAngle(fallbackAxis,AX_PI);
				}
				else
				{
					// Generate an axis
					Vector3 axis = Vector3(1.0f,0.0f,0.0f) ^ v0;
					if (axis.isZero()) // pick another if colinear
						axis = Vector3(0.0f,1.0f,0.0f) ^ v0;
					axis.normalize();
                    fromAxisAngle(axis,AX_PI);
				}
			}
			else
			{
				float s = sqrt((1+d)*2);
				float invs = 1 / s;

				Vector3 c = v0 ^ v1;
                
				x = c.x * invs;
				y = c.y * invs;
				z = c.z * invs;
				w = s * 0.5;
			
				normalize();
			}
		}

		inline void computeW() {
			float t = 1.0f - (x * x) - (y * y) - (z * z);

			if (t < 0.0f)
				w = 0.0f;
			else
				w = sqrt(t);
		}

		inline void fromAxisAngle(const Vector3 &axis, float angle) {
			float sin_a = sin(angle * AX_D2R * 0.5f);
			float cos_a = cos(angle * AX_D2R * 0.5f);
			x = axis.x * sin_a;
			y = axis.y * sin_a;
			z = axis.z * sin_a;
			w = cos_a;
		}

		inline Quaternion slerp(const Quaternion &to, float t) const {
			Quaternion temp;
			float omega, cosom, sinom, scale0, scale1;

			cosom = x * to.x + y * to.y + z * to.z + w * to.w;
			if (cosom < 0.0f) {
				temp = -to;
				cosom = -cosom;
			} else {
				temp = to;
			}

			if ((1.0f - cosom) > 1e-6f) {
				scale0 = 1.0f - cosom * cosom;
				sinom = Math::rsqrt(scale0);
				omega = atan2f(scale0 * sinom, cosom);
				scale0 = sinf((1.0f - t) * omega) * sinom;
				scale1 = sinf(t * omega) * sinom;
			} else {
				scale0 = 1.0f - t;
				scale1 = t;
			}

			temp = (*this * scale0) + (temp * scale1);
			return temp;
		}

		inline float length() {
			float len;
			len = x * x + y * y + z * z + w * w;
			return sqrtf(len);
		}

		inline void normalize() {
			float len;
			float ilength;

			len = this->length();
			if (len) {
				ilength = 1 / len;
				x *= ilength;
				y *= ilength;
				z *= ilength;
				w *= ilength;
			}
		}
	};

	struct AX_API CompactQuat {
		float x, y, z;

		inline CompactQuat(void) {}
		inline CompactQuat(float m_x, float m_y, float m_z) : x(m_x), y(m_y), z(m_z) {}

		void 			set(float m_x, float m_y, float m_z);

		float operator[](int index) const;
		float &operator[](int index);

		bool compare(const CompactQuat &a) const;
		bool compare(const CompactQuat &a, const float epsilon) const;
		bool operator==(	const CompactQuat &a) const;
		bool operator!=(	const CompactQuat &a) const;

		int getDimension(void) const;

		Quaternion toQuaternion(void) const;
		const float * toFloatPtr(void) const;
		float * toFloatPtr(void);
	};

	inline void CompactQuat::set(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline float CompactQuat::operator[](int index) const {
		AX_STRICT_ASSERT((index >= 0) && (index < 3));
		return (&x)[ index ];
	}

	inline float &CompactQuat::operator[](int index) {
		AX_STRICT_ASSERT((index >= 0) && (index < 3));
		return (&x)[ index ];
	}

	inline bool CompactQuat::compare(const CompactQuat &a) const {
		return ((x == a.x) && (y == a.y) && (z == a.z));
	}

	inline bool CompactQuat::compare(const CompactQuat &a, const float epsilon) const {
		if (fabs(x - a.x) > epsilon) {
			return false;
		}
		if (fabs(y - a.y) > epsilon) {
			return false;
		}
		if (fabs(z - a.z) > epsilon) {
			return false;
		}
		return true;
	}

	inline bool CompactQuat::operator==(const CompactQuat &a) const {
		return compare(a);
	}

	inline bool CompactQuat::operator!=(const CompactQuat &a) const {
		return !compare(a);
	}

	inline int CompactQuat::getDimension(void) const {
		return 3;
	}

	inline Quaternion CompactQuat::toQuaternion(void) const {
		return Quaternion(x, y, z, sqrt(fabs(1.0f - (x * x + y * y + z * z))));
	}

	inline const float *CompactQuat::toFloatPtr(void) const {
		return &x;
	}

	inline float *CompactQuat::toFloatPtr(void) {
		return &x;
	}

	inline void Quaternion::fromCompact(const CompactQuat &cquat) {
		x = cquat.x;
		y = cquat.y;
		z = cquat.z;
		computeW();
	}


AX_END_NAMESPACE

#endif // end guardian

