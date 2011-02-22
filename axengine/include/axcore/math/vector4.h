/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_VECTOR4_H
#define AX_CORE_VECTOR4_H

AX_BEGIN_NAMESPACE

struct AX_API Vector4 {
	float x,y,z,w;

	static const Vector4 One;
	static const Vector4 Zero;

	// ctor and dtor
	inline Vector4(){}
	inline Vector4(const Vector3 &v, float iw) : x(v.x),y(v.y),z(v.z), w(iw) {}
	inline Vector4(const Vector4 &p) : x(p.x),y(p.y),z(p.z), w(p.w) {}
	inline Vector4(float ix, float iy, float iz, float iw) : x(ix) , y(iy), z(iz), w(iw) {}
	inline Vector4(const Vector3 &base, const Vector3 &normal)
		: x(normal.x), y(normal.y), z(normal.z)
		, w(-(base|normal)) {}
	inline Vector4(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
		Vector3 n = ((b-a)^(c-a)).getNormalized();
		float d = -(a | n);
		*this = Vector4(n, d);
	}
	inline ~Vector4(){}

	// operator
	inline bool operator==(const Vector4 &p) const  {
		return x==p.x && y==p.y && z==p.z && w==p.w;
	}

	inline bool operator!=(const Vector4 &p) const {
		return x!=p.x || y!=p.y || z!=p.z || w!=p.w;
	}

	inline Vector4 operator+(const Vector4 &p) const {
		return Vector4(x + p.x, y + p.y, z + p.z, w + p.w);
	}

	inline Vector4 operator+(const Vector2 &rhs) const {
		return Vector4(x + rhs.x, y + rhs.y, z + rhs.x, w + rhs.y);
	}

	inline Vector4 operator-() const {
		return Vector4(-x, -y, -z, -w);
	}

	inline Vector4 operator-(const Vector4 &p) const {
		return Vector4(x - p.x, y - p.y, z - p.z, w - p.w);
	}

	inline Vector4 operator-(const Vector2 &rhs) const {
		return operator+(-rhs);
	}

	inline Vector4 operator/(float scale) const {
		float rs = 1.f/scale;
		return Vector4(x * rs, y * rs, z * rs, w * rs);
	}

	inline Vector4 operator*(float scale) const {
		return Vector4(x * scale, y * scale, z * scale, w * scale);
	}

	inline Vector4 operator*(const Vector2 &rhs) const {
		return Vector4(x * rhs.x, y * rhs.y, z * rhs.x, w * rhs.y);
	}

	inline Vector4 operator/(const Vector2 &rhs) const {
		Vector2 local(1.0f / rhs.x, 1.0f / rhs.y);
		return operator*(local);
	}

	// Dot Product
	inline float operator|(const Vector4 &p) const {
		return x*p.x + y*p.y + z*p.z + w*p.w;
	}
	inline Vector4 operator+=(const Vector4 &p) {
		x += p.x; y += p.y; z += p.z; w += p.w;
		return *this;
	}
	inline Vector4 operator-=(const Vector4 &p) {
		x -= p.x; y -= p.y; z -= p.z; w -= p.w;
		return *this;
	}
	inline Vector4 operator*=(float scale) {
		x *= scale; y *= scale; z *= scale; w *= scale;
		return *this;
	}
	inline Vector4 operator*=(const Vector4 &p) {
		x *= p.x; y *= p.y; z *= p.z; w *= p.w;
		return *this;
	}
	inline Vector4 operator/=(float p) {
		float rv = 1.f/p;
		x *= rv; y *= rv; z *= rv; w *= rv;
		return *this;
	}
	inline float &operator[](int index) {
		AX_ASSERT(index>=0 && index<4);
		return *(&x+index);
	}
	inline const float &operator[](int index) const {
		AX_ASSERT(index>=0 && index<4);
		return *(&x+index);
	}
	inline float &operator[](size_t index) {
		AX_ASSERT(index>=0 && index<4);
		return *(&x+index);
	}
	inline const float &operator[](size_t index) const {
		AX_ASSERT(index>=0 && index<4);
		return *(&x+index);
	}
	inline void set(float ix, float iy, float iz, float iw)
	{
		x = ix; y = iy; z = iz; w = iw;
	}
	inline void set(const float *ptr)
	{
		x = ptr[0]; y = ptr[1]; z = ptr[2]; w = ptr[3];
	}
	inline const float* c_ptr() const
	{
		return &x;
	}
	inline float mapping(Vector3 &p)
	{
		return x*p.x + y*p.y + z*p.z - w;
	}
	inline Vector3 &xyz()
	{
		return *(Vector3*)this;
	}

	inline Vector2 &xy()
	{
		return *(Vector2*)this;
	}

	inline Vector2 &zw()
	{
		return *(Vector2*)&z;
	}

	inline const Vector3 &xyz() const
	{
		return *(Vector3*)this;
	}
};

AX_END_NAMESPACE

#endif // end guardian

