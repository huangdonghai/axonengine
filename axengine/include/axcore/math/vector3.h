/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_VECTOR3_H
#define AX_CORE_VECTOR3_H

AX_BEGIN_NAMESPACE

struct AX_API Vector3 {
	float x, y, z;

	static const Vector3 One;
	static const Vector3 Zero;

	// constructor and destructor
	inline Vector3() {}
	inline Vector3(float ix, float iy, float iz) : x(ix), y(iy), z(iz) {}
	inline Vector3(const Vector2 &v, float iz) : x(v.x), y(v.y), z(iz) {}
	inline ~Vector3() {}

	// algo
	inline Vector3 operator+(const Vector3 &v) const {
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	// negative vector
	inline Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}
	inline Vector3 operator-(const Vector3 &v) const {
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	// scale the vector
	inline Vector3 operator*(const float scale) const {
		return Vector3(x*scale, y*scale, z*scale);
	}
	// scale
	inline Vector3 operator*(const Vector3 &v) const {
		return Vector3(x*v.x, y*v.y, z*v.z);
	}
	inline Vector3 operator/(const Vector3 &v) const {
		return Vector3(x/v.x, y/v.y, z/v.z);
	}
	// inverse scale
	inline Vector3 operator/(float scale) const {
		return operator*(1.f / scale);
	}
	// Dot Product
	inline float operator|(const Vector3 &v) const {
		return x*v.x + y*v.y + z*v.z;
	}
	// Cross Product
	inline Vector3 operator^(const Vector3 &v) const {
		return Vector3(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
			);
	}
	// compare
	inline bool operator==(const Vector3 &v) const {
		return x==v.x && y==v.y && z==v.z;
	}
	bool operator!=(const Vector3 &v) const {
		return x!=v.x || y!=v.y || z!=v.z;
	}

	// assign operator
	inline Vector3 operator+=(const Vector3 &v) {
		x += v.x; y += v.y; z += v.z;
		return *this;
	}
	inline Vector3 operator-=(const Vector3 &v) {
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}
	inline Vector3 operator*=(float scale) {
		x *= scale; y *= scale; z *= scale;
		return *this;
	}
	inline Vector3 operator/=(float v) {
		v = 1.f/v;
		x *= v; y *= v; z *= v;
		return *this;
	}
	inline Vector3 operator*=(const Vector3 &v) {
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
	}
	inline Vector3 operator/=(const Vector3 &v) {
		x /= v.x; y /= v.y; z /= v.z;
		return *this;
	}
	inline float &operator[](int index) {
		AX_STRICT_ASSERT(index >= 0 && index < 3);
		return *(&x+index);
	}

	inline const float &operator[](int index) const {
		AX_STRICT_ASSERT(index >= 0 && index < 3);
		return *(&x+index);
	}

	// Simple functions.
	inline float getMax() const {
		return std::max(std::max(x,y), z);
	}
	inline float getAbsMax() const {
		return std::max(std::max(Math::abs(x), Math::abs(y)), Math::abs(z));
	}
	inline float getMin() const {
		return std::min(std::min(x,y),z);
	}
	inline float getLength() const {
		return (float)sqrt(x*x + y*y + z*z);
	}
	inline float getLengthSquared() const {
		return x*x + y*y + z*z;
	}
	inline bool isZero() const {
		return x==0.f && y==0.f && z==0.f;
	}
	// return length
	inline float normalize() {
		float len = getLength();
		if (len >= 1.0e-16f) {
			float scale = 1.f/len;
			x *= scale; y *= scale; z *= scale;
			return len;
		} else {
			x=y=z=0.f;
			return 0.f;
		}
	}
	// return self value
	inline Vector3 getNormalized() const {
		float s = x*x+y*y+z*z;
		if (s >= 1.0e-16f) {
			float scale = 1.f/sqrt(s);
			return Vector3(x*scale,y*scale,z*scale);
		} else {
			return Vector3(0,0,0);
		}
	}
	// Clear self
	inline void clear() {
		x=0.0f;y=0.0f;z=0.0f;
	}
	// Set value
	inline Vector3 &set(float ix=0.0f, float iy=0.0f, float iz=0.0f) {
		x = ix; y = iy; z = iz;
		return *this;
	}

	inline Vector3 &set(const float *p) {
		x = p[0]; y = p[1]; z = p[2];
		return *this;
	}
	inline operator const float*() const {
		return &x;
	}

	inline operator Vector2 () const {
		return Vector2(x,y);
	}

	inline Vector2 xy() const {
		return Vector2(x, y);
	}

	String toString() const;
	bool fromString(const char *str);
};


AX_END_NAMESPACE

#endif // end guardian

