/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_PLANE_H
#define AX_CORE_PLANE_H

AX_BEGIN_NAMESPACE

// a plane a * x + b * y + c * z + d = 0
struct AX_API Plane {
	enum Side { Invalid = 0, Front=1, Back=2, On=4, Cross=Front+Back };

	float a, b, c, d;

	// ctor and dtor
	Plane();
	Plane(const Vector3 &v, float dist=1);
	Plane(const Plane &p);
	Plane(float a, float b, float c, float d);
	Plane(const Vector3 &base, const Vector3 &normal);
	Plane(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2);
	~Plane();

	// operator
	bool operator==(const Plane &p) const;
	bool operator!=(const Plane &p) const;
	Plane operator+(const Plane &p) const;
	Plane operator-() const;
	Plane operator-(const Plane &p) const;
	Plane operator/(float scale) const;
	Plane operator*(float scale) const;
	float operator|(const Plane &p) const;
	Plane operator+=(const Plane &p);
	Plane operator-=(const Plane &p);
	Plane operator*=(float scale);
	Plane operator*=(const Plane &p);
	Plane operator/=(float p);
	float &operator[](int index);
	float operator[](int index) const;
	float &operator[](size_t index);
	float operator[](size_t index) const;
	Plane &set(float a, float b, float c, float d=0);
	operator const float*() const;
	float mapping(Vector3 &p);
	Vector3 &normal();
	const Vector3 &getNormal() const;
	void setNormal(const Vector3 &n);
	void fitThroughPoint(const Vector3 &p);

	Side side(const BoundingBox &bbox) const;
	float distance(const Vector3 &v) const;
	bool lineIntersection(const Vector3 &start, const Vector3 &end) const;
	bool rayIntersection(const Vector3 &start, const Vector3 &dir, float &scale) const;
};

inline Plane::Plane(){}
inline Plane::Plane(const Vector3 &v, float dist) : a(v.x),b(v.y),c(v.z), d(dist) {}
inline Plane::Plane(const Plane &p) : a(p.a),b(p.b),c(p.c), d(p.d) {}
inline Plane::Plane(float ix, float iy, float iz, float iw) : a(ix) , b(iy), c(iz), d(iw) {}
inline Plane::Plane(const Vector3 &base, const Vector3 &normal)
	: a(normal.x), b(normal.y), c(normal.z)
	, d(-(base|normal)) {}
inline Plane::Plane(const Vector3 &p0, const Vector3 &p1, const Vector3 &p2) {
	Vector3 n = ((p1-p0)^(p2-p0)).getNormalized();
	float d = -(p0 | n);
	*this = Plane(n, d);
}
inline Plane::~Plane(){}

// operator
inline bool Plane::operator==(const Plane &p) const  {
	return a==p.a && b==p.b && c==p.c && d==p.d;
}
inline bool Plane::operator!=(const Plane &p) const {
	return a!=p.a || b!=p.b || c!=p.c || d!=p.d;
}
inline Plane Plane::operator+(const Plane &p) const {
	return Plane(a + p.a, b + p.b, c + p.c, d + p.d);
}
inline Plane Plane::operator-() const {
	return Plane(-a, -b, -c, -d);
}
inline Plane Plane::operator-(const Plane &p) const {
	return Plane(a - p.a, b - p.b, c - p.c, d - p.d);
}
inline Plane Plane::operator/(float scale) const {
	float rs = 1.f/scale;
	return Plane(a * rs, b * rs, c * rs, d * rs);
}

inline Plane Plane::operator*(float scale) const {
	return Plane(a * scale, b * scale, c * scale, d * scale);
}
// Dot Product
inline float Plane::operator|(const Plane &p) const {
	return a*p.a + b*p.b + c*p.c + d*p.d;
}
inline Plane Plane::operator+=(const Plane &p) {
	a += p.a; b += p.b; c += p.c; d += p.d;
	return *this;
}
inline Plane Plane::operator-=(const Plane &p) {
	a -= p.a; b -= p.b; c -= p.c; d -= p.d;
	return *this;
}
inline Plane Plane::operator*=(float scale) {
	a *= scale; b *= scale; c *= scale; d *= scale;
	return *this;
}
inline Plane Plane::operator*=(const Plane &p) {
	a *= p.a; b *= p.b; c *= p.c; d *= p.d;
	return *this;
}
inline Plane Plane::operator/=(float p) {
	float rv = 1.f/p;
	a *= rv; b *= rv; c *= rv; d *= rv;
	return *this;
}
inline float &Plane::operator[](int index) {
	AX_STRICT_ASSERT(index>=0 && index<4);
	return *(&a+index);
}
inline float Plane::operator[](int index) const {
	AX_STRICT_ASSERT(index>=0 && index<4);
	return *(&a+index);
}
inline float &Plane::operator[](size_t index) {
	AX_STRICT_ASSERT(index>=0 && index<4);
	return *(&a+index);
}
inline float Plane::operator[](size_t index) const {
	AX_STRICT_ASSERT(index>=0 && index<4);
	return *(&a+index);
}
inline Plane &Plane::set(float ix, float iy, float iz, float iw) {
	a = ix; b = iy; c = iz; d = iw;
	return *this;
}
inline Plane::operator const float*() const {
	return &a;
}
inline float Plane::mapping(Vector3 &p) {
	return a*p.x + b*p.y + c*p.z - d;
}
inline Vector3 &Plane::normal() {
	return *(Vector3*)this;
}
inline const Vector3 &Plane::getNormal() const {
	return *(Vector3*)this;
}
inline void Plane::setNormal(const Vector3 &n) {
	a = n.x; b = n.y; c = n.z;
}
inline void Plane::fitThroughPoint(const Vector3 &p) {
	d = -(normal() | p);
}
inline float Plane::distance(const Vector3 &v) const {
	return a * v.x + b * v.y + c * v.z + d;
}
inline bool Plane::lineIntersection(const Vector3 &start, const Vector3 &end) const {
	float d1, d2, fraction;

	d1 = (getNormal() | start) + d;
	d2 = (getNormal() | end) + d;
	if (d1 == d2) {
		return false;
	}
	if (d1 > 0.0f && d2 > 0.0f) {
		return false;
	}
	if (d1 < 0.0f && d2 < 0.0f) {
		return false;
	}
	fraction = (d1 / (d1 - d2));
	return (fraction >= 0.0f && fraction <= 1.0f);
}

inline bool Plane::rayIntersection(const Vector3 &start, const Vector3 &dir, float &scale) const {
	float d1, d2;

	d1 = (getNormal() | start) + d;
	d2 = (getNormal() | dir);
	if (d2 == 0.0f) {
		return false;
	}
	scale = -(d1 / d2);
	return true;
}


AX_END_NAMESPACE

#endif // end guardian

