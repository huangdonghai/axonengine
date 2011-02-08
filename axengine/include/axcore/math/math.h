/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CORE_MATH_H
#define AX_CORE_MATH_H

/* Definitions of useful mathematical constants
 * AX_E - e
 * AX_LOG2E - log2(e)
 * AX_LOG10E - log10(e)
 * AX_LN2 - ln(2)
 * AX_LN10 - ln(10)
 * AX_PI - pi
 * AX_PI_2 - pi/2
 * AX_PI_4 - pi/4
 * AX_1_PI - 1/pi
 * AX_2_PI - 2/pi
 * AX_2_SQRTPI - 2/sqrt(pi)
 * AX_SQRT2 - sqrt(2)
 * AX_SQRT1_2 - 1/sqrt(2)
 * AX_D2R - pi/180
 * AX_R2D - 180/pi
 */
 
#define AX_E 2.71828182845904523536f
#define AX_LOG2E 1.44269504088896340736f
#define AX_LOG10E 0.434294481903251827651f
#define AX_LN2 0.693147180559945309417f
#define AX_LN10 2.30258509299404568402f
#define AX_PI 3.14159265358979323846f
#define AX_PI_2 1.57079632679489661923f
#define AX_PI_4 0.785398163397448309616f
#define AX_1_PI 0.318309886183790671538f
#define AX_2_PI 0.636619772367581343076f
#define AX_2_SQRTPI 1.12837916709551257390f
#define AX_SQRT2 1.41421356237309504880f
#define AX_SQRT1_2 0.707106781186547524401f
#define AX_D2R 0.0174532925199432957692f /* == PI / 180 */
#define AX_R2D 57.295779513082320876798f /* == 180 / PI */

#define FLOATSIGNBITSET(f)		((*(const unsigned long *)&(f)) >> 31)
#define FLOATSIGNBITNOTSET(f)	((~(*(const unsigned long *)&(f))) >> 31)
#define FLOATNOTZERO(f)			((*(const unsigned long *)&(f)) & ~(1<<31) )

#define INTSIGNBITSET(i)		(((const unsigned long)(i)) >> 31)
#define INTSIGNBITNOTSET(i)		((~((const unsigned long)(i))) >> 31)

AX_BEGIN_NAMESPACE

// forward declaration
struct Vector2;
struct Vector3;
struct Vector4;
struct Plane;
struct Matrix2;
struct Matrix3;
struct Matrix4;
struct Point;
struct Rect;
struct Quaternion;
struct Angles;
struct BoundingBox;
struct Matrix;
struct Rgb;
struct Rgba;
struct Color3;
struct Color4;

#if 0
struct Bgr;
struct Bgra;
#endif

struct AX_API Math
{
	enum Constant {
		IEEE_FLT_MANTISSA_BITS = 23,
		IEEE_FLT_EXPONENT_BITS = 8,
		IEEE_FLT_EXPONENT_BIAS = 127,
		IEEE_FLT_SIGN_BIT = 31
	};

	static void initialize();

	static float rsqrt(float x);
	static void sincos(float a, float &s, float &c);		// sine and cosine with 32 bits precision
	static void sincos16(float a, float &s, float &c);	// sine and cosine with 16 bits precision
	static void sincos64(float a, double &s, double &c);	// sine and cosine with 64 bits precision

	static float normalizeDegree(float degree);
	static float d2r(float in);
	static float r2d(float in);
	static int nextPowerOfTwo(int x);
	static int previousPowerOfTwo(const int v);
	static int nearestPowerOfTwo(const int v);
	static bool isPowerOfTwo(int n);
	static int countMipmaps(int w, int h, int d);

	// map a value to range [0, width)
	static int mapToRange(int v, int l, int h);
	static int mapToBound(int n, int width);

	// 获取checkPoint到线段p1, p2的最短距离  timlly add
	static float getNearstDistanceSegment(const Vector3 &p1, const Vector3 &p2, const Vector3 &checkPoint);
	// 获取checkPoint到直线p1, p2的最短距离  timlly add
	static float getNearstDistanceLine(const Vector3 &p1, const Vector3 &p2, const Vector3 &checkPoint);

	// clamp
	static int clampSbyte(int n);
	static int clampByte(int n);
	static int clampUshort(int n);
	static float saturate(float f);

	static int floatToBits(float f, int exponentBits, int mantissaBits);
	static float bitsToFloat(int i, int exponentBits, int mantissaBits);

	static int angleToByte(float angle) { return (int)(angle * 256.0f / 360.0f ) & 255; }
	static float byteToAngle(int x) { return (x) * ( 360.0f / 256.0f );}

	static int angleToShort(float angle) { return (int)(angle * 65536.0f / 360.0f ) & 65535; }
	static float shortToAngle(int x) { return (x) * ( 360.0f / 65536.0f );}

	template<typename T>
	static T abs(T x);

	template<typename T>
	static T clamp(T v, T l, T h);

	template<typename T>
	static T sign(T x);

	static size_t distant(size_t baseline, size_t pos)
	{
		if (pos >= baseline) return pos - baseline;
		return pos + ~baseline + 1;
	}

};

// 1 / sqrt(x)
inline float Math::rsqrt(float x)
{
	long i;
	float y, r;

	y = x * 0.5f;
	i = *reinterpret_cast<long *>(&x);
	i = 0x5f3759df - (i >> 1);
	r = *reinterpret_cast<float *>(&i);
	r = r * (1.5f - r * r * y);
	return r;
}

inline void Math::sincos(float a, float &s, float &c)
{
#ifdef _WIN32
	_asm {
		fld a
		fsincos
		mov ecx, c
		mov edx, s
		fstp dword ptr [ecx]
		fstp dword ptr [edx]
	}
#else
	s = sinf(a);
	c = cosf(a);
#endif
}

inline void Math::sincos16(float a, float &s, float &c)
{
	float t, d;

	const float pi2 = AX_PI * 2.0f;
	if ((a < 0.0f) || (a >= pi2)) {
		a -= floorf(a / pi2) * pi2;
	}

	if (a < AX_PI) {
		if (a > AX_PI_2) {
			a = AX_PI - a;
			d = -1.0f;
		} else {
			d = 1.0f;
		}
	} else {
		if (a > AX_PI + AX_PI_2) {
			a = a - pi2;
			d = 1.0f;
		} else {
			a = AX_PI - a;
			d = -1.0f;
		}
	}

	t = a * a;
	s = a * (((((-2.39e-08f * t + 2.7526e-06f) * t - 1.98409e-04f) * t + 8.3333315e-03f) * t - 1.666666664e-01f) * t + 1.0f);
	c = d * (((((-2.605e-07f * t + 2.47609e-05f) * t - 1.3888397e-03f) * t + 4.16666418e-02f) * t - 4.999999963e-01f) * t + 1.0f);
}

inline void Math::sincos64(float a, double &s, double &c)
{
#ifdef _WIN32
	_asm {
		fld a
		fsincos
		mov ecx, c
		mov edx, s
		fstp qword ptr [ecx]
		fstp qword ptr [edx]
	}
#else
	s = sin(a);
	c = cos(a);
#endif
}

inline int Math::mapToRange(int v, int l, int h)
{
	v = v - l;
	int len = h - l;

	int mod =  v % len;
	if (mod >= 0)
		return mod;
	return len + mod;
}

inline float Math::d2r(float in)
{
	return in * AX_D2R;
}

inline float Math::r2d(float in)
{
	return in * AX_R2D;
}

/** Return the next power of two. 
* @see http://graphics.stanford.edu/~seander/bithacks.html
* @warning Behaviour for 0 is undefined.
* @note isPowerOfTwo(x) == true -> nextPowerOfTwo(x) == x
* @note nextPowerOfTwo(x) = 2 << log2(x-1)
*/
inline int Math::nextPowerOfTwo(int x)
{
	if(!x) return 1;
	// On modern CPUs this is as fast as using the bsr instruction.
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x+1;	
}

/// Return true if @a n is a power of two.
inline bool Math::isPowerOfTwo(int n)
{
	return (n & (n-1)) == 0;
}

inline int Math::countMipmaps(int w, int h, int d)
{
	int mipmap = 0;

	while (w != 1 || h != 1 || d != 1) {
		w = std::max(1, w / 2);
		h = std::max(1, h / 2);
		d = std::max(1, d / 2);
		mipmap++;
	}

	return mipmap + 1;
}

// 1 -> 1, 2 -> 2, 3 -> 2, 4 -> 4, 5 -> 4, ...
inline int Math::previousPowerOfTwo(const int v)
{
	return nextPowerOfTwo(v + 1) / 2;
}

inline int Math::nearestPowerOfTwo(const int v)
{
	const int np2 = nextPowerOfTwo(v);
	const int pp2 = previousPowerOfTwo(v);

	if (np2 - v <= v - pp2) {
		return np2;
	} else {
		return pp2;
	}
}

// map a value to range [0, width)
inline int Math::mapToBound(int n, int width)
{
	n = n % width;
	if (n < 0)
		n += width;

	return n;
}

inline int Math::clampByte(int n)
{
	return clamp<int>(n, 0, 255);
}

inline int Math::clampSbyte(int n)
{
	return clamp<int>(n, -128, 127);
}

inline int Math::clampUshort(int n)
{
	return clamp<int>(n, 0, 0xffff);
}

inline float Math::saturate(float f)
{
	return clamp(f, 0.0f, 1.0f);
}

template<typename T>
T Math::abs(T x)
{
	return x > 0 ? x : -x;
}

template<typename T>
T Math::clamp(T v, T l, T h)
{
	return ((v) < (l) ? (l) : (v) > (h) ? (h) : (v));
}

template<typename T>
T Math::sign(T x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}

AX_END_NAMESPACE

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "color.h"
#include "quaternion.h"
#include "matrix2.h"
#include "matrix3.h"
#include "bounds.h"
#include "plane.h"
#include "matrix4.h"
#include "point.h"
#include "size.h"
#include "rect.h"
#include "angles.h"
#include "vertex.h"
#include "rotation.h"
#include "matrix.h"
#include "simd.h"
#include "perlinnoise.h"
#include "spline.h"

#endif // AX_CORE_MATH_H
