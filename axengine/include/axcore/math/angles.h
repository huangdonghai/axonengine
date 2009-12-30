/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_ANGLES_H
#define AX_CORE_ANGLES_H

AX_BEGIN_NAMESPACE

	// Euler angles

	struct AX_API Angles {
		enum { Pitch, Yaw, Roll };

		float pitch, yaw, roll;

		Angles(void);
		Angles(float pitch, float yaw, float roll);
		Angles(const Vector3 &v);

		void set(float pitch, float yaw, float roll);
		Angles& zero(void);

		float operator[](int index) const;
		float& operator[](int index);
		Angles operator-() const;			// negate angles, in general not the inverse rotation
		Angles& operator=(const Angles &a);
		Angles operator+(const Angles &a) const;
		Angles& operator+=(const Angles &a);
		Angles operator-(const Angles &a) const;
		Angles& operator-=(const Angles &a);
		Angles operator*(const float a) const;
		Angles& operator*=(const float a);
		Angles operator/(const float a) const;
		Angles& operator/=(const float a);

		friend Angles operator*(const float a, const Angles &b);

		bool compare(const Angles &a) const;							// exact compare, no epsilon
		bool compare(const Angles &a, const float epsilon) const;	// compare with epsilon
		bool operator==(const Angles &a) const;						// exact compare, no epsilon
		bool operator!=(const Angles &a) const;						// exact compare, no epsilon

		Angles& normalize360(void);	// normalizes 'this'
		Angles& normalize180(void);	// normalizes 'this'

		void clamp(const Angles &min, const Angles &max);

		int getDimension(void) const;

		Vector3 toVector3() const { return *(Vector3*)this; }

		void toVectors(Vector3 *forward, Vector3 *right = NULL, Vector3 *up = NULL) const;
		Vector3 toForward(void) const;
		Quaternion toQuaternion(void) const;
		Matrix3 toMatrix3(void) const;
		Matrix4 toMatrix4(void) const;
		Vector3 toAngularVelocity(void) const;
		const float* toFloatPtr(void) const;
		float* toFloatPtr(void);

		Angles& fromShort(short rhs[3]);
		static Angles trShort(short rhs[3]);
	};

	inline Angles::Angles(void) {
	}

	inline Angles::Angles(float pitch, float yaw, float roll) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
	}

	inline Angles::Angles(const Vector3 &v) {
		this->pitch = v[0];
		this->yaw = v[1];
		this->roll = v[2];
	}

	inline void Angles::set(float pitch, float yaw, float roll) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
	}

	inline Angles &Angles::zero(void) {
		pitch = yaw = roll = 0.0f;
		return *this;
	}

	inline float Angles::operator[](int index) const {
		AX_STRICT_ASSERT((index >= 0) && (index < 3));
		return (&pitch)[ index ];
	}

	inline float &Angles::operator[](int index) {
		AX_STRICT_ASSERT((index >= 0) && (index < 3));
		return (&pitch)[ index ];
	}

	inline Angles Angles::operator-() const {
		return Angles(-pitch, -yaw, -roll);
	}

	inline Angles &Angles::operator=(const Angles &a) {
		pitch = a.pitch;
		yaw = a.yaw;
		roll = a.roll;
		return *this;
	}

	inline Angles Angles::operator+(const Angles &a) const {
		return Angles(pitch + a.pitch, yaw + a.yaw, roll + a.roll);
	}

	inline Angles& Angles::operator+=(const Angles &a) {
		pitch += a.pitch;
		yaw += a.yaw;
		roll += a.roll;

		return *this;
	}

	inline Angles Angles::operator-(const Angles &a) const {
		return Angles(pitch - a.pitch, yaw - a.yaw, roll - a.roll);
	}

	inline Angles& Angles::operator-=(const Angles &a) {
		pitch -= a.pitch;
		yaw -= a.yaw;
		roll -= a.roll;

		return *this;
	}

	inline Angles Angles::operator*(const float a) const {
		return Angles(pitch * a, yaw * a, roll * a);
	}

	inline Angles& Angles::operator*=(float a) {
		pitch *= a;
		yaw *= a;
		roll *= a;
		return *this;
	}

	inline Angles Angles::operator/(const float a) const {
		float inva = 1.0f / a;
		return Angles(pitch * inva, yaw * inva, roll * inva);
	}

	inline Angles& Angles::operator/=(float a) {
		float inva = 1.0f / a;
		pitch *= inva;
		yaw *= inva;
		roll *= inva;
		return *this;
	}

	inline Angles operator*(float a, const Angles &b) {
		return b * a;
	}

	inline bool Angles::compare(const Angles &a) const {
		return (a.pitch == pitch) && ((a.yaw == yaw) && (a.roll == roll));
	}

	inline bool Angles::compare(const Angles &a, const float epsilon) const {
		if (fabs(pitch - a.pitch) > epsilon) {
			return false;
		}

		if (fabs(yaw - a.yaw) > epsilon) {
			return false;
		}

		if (fabs(roll - a.roll) > epsilon) {
			return false;
		}

		return true;
	}

	inline bool Angles::operator==(const Angles &a) const {
		return compare(a);
	}

	inline bool Angles::operator!=(const Angles &a) const {
		return !compare(a);
	}

	inline void Angles::clamp(const Angles &min, const Angles &max) {
		if (pitch < min.pitch) {
			pitch = min.pitch;
		} else if (pitch > max.pitch) {
			pitch = max.pitch;
		}
		if (yaw < min.yaw) {
			yaw = min.yaw;
		} else if (yaw > max.yaw) {
			yaw = max.yaw;
		}
		if (roll < min.roll) {
			roll = min.roll;
		} else if (roll > max.roll) {
			roll = max.roll;
		}
	}

	inline int Angles::getDimension(void) const {
		return 3;
	}

	inline const float *Angles::toFloatPtr(void) const {
		return (float*)this;
	}

	inline float *Angles::toFloatPtr(void) {
		return (float*)this;
	}


AX_END_NAMESPACE

#endif // end guardian

