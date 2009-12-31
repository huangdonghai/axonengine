/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

	void Math::initialize() {
		return;

		//Set the x86 floating-point control word according to what
		//exceptions you want to trap. 
		_clearfp(); //Always call _clearfp before setting the control
		//word
		//Because the second parameter in the following call is 0, it
		//only returns the floating-point control word

		unsigned int cw = _controlfp(0, 0); //Get the default control

		//word
		//Set the exception masks off for exceptions that you want to
		//trap.  When a mask bit is set, the corresponding floating-point
		//exception is //blocked from being generating.
		cw &=~(EM_OVERFLOW|EM_UNDERFLOW|EM_ZERODIVIDE|
			EM_DENORMAL|EM_INVALID);
		//For any bit in the second parameter (mask) that is 1, the 
		//corresponding bit in the first parameter is used to update
		//the control word.  
		unsigned int cwOriginal = _controlfp(cw, MCW_EM); //Set it.
		//MCW_EM is defined in float.h.
		//Restore the original value when done:
		//_controlfp(cwOriginal, MCW_EM);
	}

	float Math::getNearstDistanceLine(const Vector3 &a, const Vector3 &b, const Vector3 &p)
	{
		if (a == b)
		{
			return (a-p).getLength();
		}
		
		Vector3 ab = b - a;
		float d = - (ab | a);

		return fabs((ab | p) + d) / ab.getLength();
	}

	float Math::getNearstDistanceSegment(const Vector3 &a, const Vector3 &b, const Vector3 &p)
	{
		
		//计算点p到线段(a,b)的距离
		double l; // length of line ab 
		double r;

		Vector3 ab = b - a;

		l = ab.getLength();
		if (l == 0.0)
			return (a-p).getLength();

		r = ((a.y - p.y)*(a.y - b.y) + (a.x - p.x)*(a.x - b.x) + (a.z - p.z)*(a.z - b.z)) / (l*l);

		if (r > 1) // perpendicular projection of P is on the forward extention of AB 
			return(std::min((p-b).getLength(), (p-a).getLength()));
		if (r < 0) // perpendicular projection of P is on the backward extention of AB 
			return(std::min((p-b).getLength(), (p-a).getLength()));

		//s = ((a.y - p.y)*(b.x - a.x) - (a.x - p.x)*(b.y - a.y) - (a.z - p.z)*(b.z - a.z)) / (l*l);
		//return fabs(s*l);

		//return getNearstDistanceLine(a, b, p);
		
		float d = - (ab | a);

		return fabs((ab | p) + d) / l;

		/*
		Vector3 ab = b - a;
		Vector3 ac = p - a;
		float f = ab | ac;

		if (f < 0) return ac.getLength();

		float d = ab | ab;

		if (f > d) return ac.getLength();

		f = f/d; 
		Vector3 D = a + (ab * f);   // c在ab线段上的投影点

		return (a - D).getLength();
		*/

		/*
		double S;//S表示面积
		double distance1,distance2,distance3,distance;
		double sdis1,sdis2,sdis3;//距离平方
		double temp;

		sdis1 = (a-p).getLengthSquared();
		sdis2 = (a-p).getLengthSquared();
		sdis3 = (b-a).getLengthSquared();
		distance1 = sqrt(sdis1);//p1和checkPoint之间的距离
		distance2 = sqrt(sdis2);//p2和checkPoint之间的距离
		distance3 = sqrt(sdis3);//p2和p1之间的距离

		if ((distance1+distance2) == distance3)//在线段内部
			distance= 0;
		else if ((distance1+distance3) == distance2)//在p2p1的延长线上
			distance= distance1 ;
		else if ((distance2+distance3) == distance1)//在p1p2的延长线上
			distance= distance2 ;
		else if ((sdis1+sdis3) < sdis2)
			distance= distance1 ;
		else if ((sdis2+sdis3) < sdis1)
			distance= distance2 ;      
		else
		{
			temp = (distance1 + distance2 +distance3)/2;
			S = sqrt(temp*(temp-distance1)*(temp-distance2)*(temp-distance3));
			distance= (2 * S) / distance3;
		}

		return (float)distance;*/
	}

	float Math::normalizeDegree(float degree) {
		degree -= floor(degree / 360.f) * 360.f;

		return degree;
	}

	int Math::floatToBits( float f, int exponentBits, int mantissaBits ) {
		int i, sign, exponent, mantissa, value;

		AX_ASSERT( exponentBits >= 2 && exponentBits <= 8 );
		AX_ASSERT( mantissaBits >= 2 && mantissaBits <= 23 );

		int maxBits = (((1 << (exponentBits - 1)) - 1 ) << mantissaBits) | ((1 << mantissaBits) - 1);
		int minBits = (((1 <<  exponentBits     ) - 2 ) << mantissaBits) | 1;

		float max = bitsToFloat( maxBits, exponentBits, mantissaBits );
		float min = bitsToFloat( minBits, exponentBits, mantissaBits );

		if (f >= 0.0f) {
			if ( f >= max ) {
				return maxBits;
			} else if ( f <= min ) {
				return minBits;
			}
		} else {
			if ( f <= -max ) {
				return ( maxBits | ( 1 << ( exponentBits + mantissaBits ) ) );
			} else if ( f >= -min ) {
				return ( minBits | ( 1 << ( exponentBits + mantissaBits ) ) );
			}
		}

		exponentBits--;
		i = *reinterpret_cast<int *>(&f);
		sign = ( i >> IEEE_FLT_SIGN_BIT ) & 1;
		exponent = ( ( i >> IEEE_FLT_MANTISSA_BITS ) & ( ( 1 << IEEE_FLT_EXPONENT_BITS ) - 1 ) ) - IEEE_FLT_EXPONENT_BIAS;
		mantissa = i & ( ( 1 << IEEE_FLT_MANTISSA_BITS ) - 1 );
		value = sign << ( 1 + exponentBits + mantissaBits );
		value |= ( ( INTSIGNBITSET( exponent ) << exponentBits ) | ( abs( exponent ) & ( ( 1 << exponentBits ) - 1 ) ) ) << mantissaBits;
		value |= mantissa >> ( IEEE_FLT_MANTISSA_BITS - mantissaBits );
		return value;
	}

	float Math::bitsToFloat( int i, int exponentBits, int mantissaBits ) {
		static int exponentSign[2] = { 1, -1 };
		int sign, exponent, mantissa, value;

		AX_ASSERT( exponentBits >= 2 && exponentBits <= 8 );
		AX_ASSERT( mantissaBits >= 2 && mantissaBits <= 23 );

		exponentBits--;
		sign = i >> ( 1 + exponentBits + mantissaBits );
		exponent = ( ( i >> mantissaBits ) & ( ( 1 << exponentBits ) - 1 ) ) * exponentSign[( i >> ( exponentBits + mantissaBits ) ) & 1];
		mantissa = ( i & ( ( 1 << mantissaBits ) - 1 ) ) << ( IEEE_FLT_MANTISSA_BITS - mantissaBits );
		value = sign << IEEE_FLT_SIGN_BIT | ( exponent + IEEE_FLT_EXPONENT_BIAS ) << IEEE_FLT_MANTISSA_BITS | mantissa;
		return *reinterpret_cast<float *>(&value);
	}

#if 0
	void Math::anglesToVectors(const Vector3 &angles, Vector3 &forward, Vector3 &left, Vector3 &up) {
		float angle;
		float sr, sp, sy, cr, cp, cy;
		// static to help MS compiler fp bugs

		angle = angles[Math::YAW] * AX_D2R;
		sy = sin(angle);
		cy = cos(angle);
		angle = angles[Math::PITCH] * AX_D2R;
		sp = sin(angle);
		cp = cos(angle);
		angle = angles[Math::ROLL] * AX_D2R;
		sr = sin(angle);
		cr = cos(angle);

		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;

		left[0] = sr * sp * cy + cr * -sy;
		left[1] = sr * sp * sy + cr * cy;
		left[2] = sr * cp;

		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}

	void Math::anglesToAxis(const Vector3 &angles, Matrix3 &axis) {
		anglesToVectors(angles, axis[0], axis[1], axis[2]);
	}
#endif
#if 0
	// project a world-space vertex to screen
	bool Math::project(Vector4 &vert, const Matrix4 &modelMatrix, const Matrix4 &projMatrix, const Vector4 &viewport) {
		vert = projMatrix * modelMatrix * vert;

		if (vert.w == 0.0f)
			return false;

		vert /= vert.w;

		vert.x = viewport[0] + (1 + vert.x) * viewport[2] / 2;
		vert.y = viewport[1] + (1 + vert.y) * viewport[3] / 2;
		vert.z = vert.z;

		return true;
	}

	// unproject a screen vertex to world-space
	bool Math::unproject(Vector4 &vert, const Matrix4 &modelMatrix, const Matrix4 &projMatrix, const Vector4 &viewport) {
		vert.x = (vert.x - viewport[0]) * 2 / viewport[2] - 1.0;
		vert.y = (vert.y - viewport[1]) * 2 / viewport[3] - 1.0;
		vert.z = vert.z;
		vert.w = 1.0;

		Matrix4 inv_mat = (projMatrix * modelMatrix).getInverse();

		vert = inv_mat * vert;
		if (vert.w == 0.0f)
			return false;

		vert /= vert.w;

		return true;
	}
	UInt Math::boxOnPlaneSide(const Vector3 &mins, const Vector3 &maxs, const Vector4 &plane) {
		int i;
		float dist1, dist2;
		UInt sides;
		Vector3 corners[2];

		for (i=0 ; i<3 ; i++) {
			if (plane[i] < 0) {
				corners[0][i] = mins[i];
				corners[1][i] = maxs[i];
			} else {
				corners[1][i] = mins[i];
				corners[0][i] = maxs[i];
			}
		}
		dist1 = (plane.xyz() | corners[0]) + plane[3];
		dist2 = (plane.xyz() | corners[1]) + plane[3];

		sides = 0;
		if (dist1 >= 0)
			sides = Math::FRONT;
		if (dist2 < 0)
			sides |= Math::BACK;

		return sides;
	}

	UInt Math::boxOnPlaneSide(const BoundingBox &bbox, const Vector4 &plane) {
		return boxOnPlaneSide(bbox.low, bbox.high, plane);
	}

	void Math::projectPointOnPlane(Vector3 &dst, const Vector3 plane, const Vector3 normal) {
		float d;
		Vector3 n;
		float inv_denom;

		inv_denom = 1.0F / (normal | normal);

		d = (normal | plane) * inv_denom;

		n = normal * inv_denom;

		dst = plane  - n * d;
	}

	bool Math::rayIntersectPlane(Vector3 &dst, const Vector3 &org, const Vector3 &dir, const Vector4 &plane) {
		float tmp = dir | plane.xyz();

		/* Is line parallel to plane? */

		if (fabs (tmp) < 1e-5f)
			return false;

		dst = dir * -( (org | plane.xyz()) + plane[3]) / tmp;
		dst += org;

		return true;
	}
#endif

	#if 0
	// Returns true if line and plane are not parallel
	bool rayPlaneIntersect(const NxRay &line, const NxPlane &plane, NxReal &distance_along_line, NxVec3 &point_on_plane)
	{
		const NxReal dn = line.dir|plane.normal;
		if (-1E-7 < dn && dn < 1E-7)	return false; // parallel

		distance_along_line = -plane.distance(line.orig)/dn;

		point_on_plane = line.orig + distance_along_line * line.dir;

		return true;
	}
	#endif

#if 0
	/* coherent noise function over 1, 2 or 3 dimensions */
	/* (copyright Ken Perlin) */

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

	static int p[B + B + 2];
	static float g3[B + B + 2][3];
	static float g2[B + B + 2][2];
	static float g1[B + B + 2];
	static int start = 1;

	static void init(void);

#define s_curve(t) (t * t * (3. - 2. * t))

#define lerp(t, a, b) (a + t * (b - a))

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

	float noise1(float arg)
	{
		int bx0, bx1;
		float rx0, rx1, sx, t, u, v, vec[1];

		vec[0] = arg;
		if (start) {
			start = 0;
			init();
		}

		setup(0, bx0,bx1, rx0,rx1);

		sx = s_curve(rx0);

		u = rx0 * g1[ p[ bx0 ] ];
		v = rx1 * g1[ p[ bx1 ] ];

		return lerp(sx, u, v);
	}

	float noise2(float vec[2])
	{
		int bx0, bx1, by0, by1, b00, b10, b01, b11;
		float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
		register int i, j;

		if (start) {
			start = 0;
			init();
		}

		setup(0, bx0,bx1, rx0,rx1);
		setup(1, by0,by1, ry0,ry1);

		i = p[ bx0 ];
		j = p[ bx1 ];

		b00 = p[ i + by0 ];
		b10 = p[ j + by0 ];
		b01 = p[ i + by1 ];
		b11 = p[ j + by1 ];

		sx = s_curve(rx0);
		sy = s_curve(ry0);

#define at2(rx,ry) (rx * q[0] + ry * q[1])

		q = g2[ b00 ] ; u = at2(rx0,ry0);
		q = g2[ b10 ] ; v = at2(rx1,ry0);
		a = lerp(sx, u, v);

		q = g2[ b01 ] ; u = at2(rx0,ry1);
		q = g2[ b11 ] ; v = at2(rx1,ry1);
		b = lerp(sx, u, v);

		return lerp(sy, a, b);
	}

	float noise3(float vec[3])
	{
		int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
		float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
		register int i, j;

		if (start) {
			start = 0;
			init();
		}

		setup(0, bx0,bx1, rx0,rx1);
		setup(1, by0,by1, ry0,ry1);
		setup(2, bz0,bz1, rz0,rz1);

		i = p[ bx0 ];
		j = p[ bx1 ];

		b00 = p[ i + by0 ];
		b10 = p[ j + by0 ];
		b01 = p[ i + by1 ];
		b11 = p[ j + by1 ];

		t  = s_curve(rx0);
		sy = s_curve(ry0);
		sz = s_curve(rz0);

#define at3(rx,ry,rz) (rx * q[0] + ry * q[1] + rz * q[2])

		q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
		q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
		a = lerp(t, u, v);

		q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
		q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
		b = lerp(t, u, v);

		c = lerp(sy, a, b);

		q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
		q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
		a = lerp(t, u, v);

		q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
		q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
		b = lerp(t, u, v);

		d = lerp(sy, a, b);

		return lerp(sz, c, d);
	}

	static void normalize2(float v[2])
	{
		float s;

		s = sqrt(v[0] * v[0] + v[1] * v[1]);
		v[0] = v[0] / s;
		v[1] = v[1] / s;
	}

	static void normalize3(float v[3])
	{
		float s;

		s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		v[0] = v[0] / s;
		v[1] = v[1] / s;
		v[2] = v[2] / s;
	}

	static void init(void)
	{
		int i, j, k;

		for (i = 0 ; i < B ; i++) {
			p[i] = i;

			g1[i] = (float)((rand() % (B + B)) - B) / B;

			for (j = 0 ; j < 2 ; j++)
				g2[i][j] = (float)((rand() % (B + B)) - B) / B;
			normalize2(g2[i]);

			for (j = 0 ; j < 3 ; j++)
				g3[i][j] = (float)((rand() % (B + B)) - B) / B;
			normalize3(g3[i]);
		}

		while (--i) {
			k = p[i];
			p[i] = p[j = rand() % B];
			p[j] = k;
		}

		for (i = 0 ; i < B + 2 ; i++) {
			p[B + i] = p[i];
			g1[B + i] = g1[i];
			for (j = 0 ; j < 2 ; j++)
				g2[B + i][j] = g2[i][j];
			for (j = 0 ; j < 3 ; j++)
				g3[B + i][j] = g3[i][j];
		}
	}
	float Math::perlinNoise(float x, UInt octaves) {
		if (octaves < 1) octaves = 1;

		float amplitude = 1.0f;
		float total_amp = 1.0f;

		float ret = 0.f;
		for (UInt i=0; i<octaves; i++) {
			ret += noise1(x) * amplitude;
			amplitude *= 0.5f;
			total_amp += amplitude;
			x *= 2.f;
		}

		return Clamp(ret/total_amp*2.0f, -1.0f, 1.0f);
	}

	float Math::perlinNoise(float x) {
		return Clamp(noise1(x)*2.0f, -1.0f, 1.0f);
	}

	float Math::perlinNoise(float x, float y, UInt octaves) {
		if (octaves < 1) octaves = 1;

		float v[2] = { x, y };

		float amplitude = 1.f;
		float total_amp = 1.0f;

		float ret = 0.f;
		for (UInt i=0; i<octaves; i++) {
			ret += noise2(v) * amplitude;
			amplitude *= 0.5f;
			total_amp += amplitude;
			v[0] *= 2.0f;
			v[1] *= 2.0f;
		}

		return Clamp(ret / total_amp*2.0f, -1.0f, 1.0f);
	}

	float Math::perlinNoise(float x, float y) {
		float v[2] = { x, y };
		float ret = noise2(v)*2.0f;

		return Clamp(ret, -1.0f, 1.0f);
	}

	float Math::perlinNoise(float x, float y, float z, UInt octaves) {
		if (octaves < 1) octaves = 1;

		float v[3] = { x, y, z };

		float amplitude = 1.f;
		float total_amp = 1.0f;

		float ret = 0.f;
		for (UInt i=0; i<octaves; i++) {
			ret += noise3(v) * amplitude;
			amplitude *= 0.5f;
			total_amp += amplitude;
			v[0] *= 2.0f;
			v[1] *= 2.0f;
			v[2] *= 2.0f;
		}

		return Clamp(ret / total_amp*2.0f, -1.0f, 1.0f);
	}

	float Math::perlinNoise(float x, float y, float z) {
		float v[3] = { x, y, z };
		float ret = noise3(v)*2.0f;

		return Clamp(ret, -1.0f, 1.0f);
	}
#endif

	/* Ray-Triangle Intersection Test Routines          */
	/* Different optimizations of my and Ben Trumbore's */
	/* code from journals of graphics tools (JGT)       */
	/* http://www.acm.org/jgt/                          */
	/* by Tomas Moller, May 2000                        */

	#include <math.h>

	#define EPSILON 0.00001f
	#define CROSS(dest,v1,v2) \
			  dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
			  dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
			  dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
	#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
	#define SUB(dest,v1,v2) \
			  dest[0]=v1[0]-v2[0]; \
			  dest[1]=v1[1]-v2[1]; \
			  dest[2]=v1[2]-v2[2]; 

	/* the original jgt code */
	int intersect_triangle(float orig[3], float dir[3],
				   float vert0[3], float vert1[3], float vert2[3],
				   float *t, float *u, float *v)
	{
	   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	   float det,inv_det;

	   /* find vectors for two edges sharing vert0 */
	   SUB(edge1, vert1, vert0);
	   SUB(edge2, vert2, vert0);

	   /* begin calculating determinant - also used to calculate U parameter */
	   CROSS(pvec, dir, edge2);

	   /* if determinant is near zero, ray lies in plane of triangle */
	   det = DOT(edge1, pvec);

	   if (det > -EPSILON && det < EPSILON)
		 return 0;
	   inv_det = 1.0 / det;

	   /* calculate distance from vert0 to ray origin */
	   SUB(tvec, orig, vert0);

	   /* calculate U parameter and test bounds */
	   *u = DOT(tvec, pvec) * inv_det;
	   if (*u < 0.0 || *u > 1.0)
		 return 0;

	   /* prepare to test V parameter */
	   CROSS(qvec, tvec, edge1);

	   /* calculate V parameter and test bounds */
	   *v = DOT(dir, qvec) * inv_det;
	   if (*v < 0.0 || *u + *v > 1.0)
		 return 0;

	   /* calculate t, ray intersects triangle */
	   *t = DOT(edge2, qvec) * inv_det;

	   return 1;
	}

	int
	intersect_triangle_oneside(float orig[3], float dir[3],
					   float vert0[3], float vert1[3], float vert2[3],
					   float *t, float *u, float *v)
	{
	   double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	   double det,inv_det;

	   /* find vectors for two edges sharing vert0 */
	   SUB(edge1, vert1, vert0);
	   SUB(edge2, vert2, vert0);

	   /* begin calculating determinant - also used to calculate U parameter */
	   CROSS(pvec, dir, edge2);

	   /* if determinant is near zero, ray lies in plane of triangle */
	   det = DOT(edge1, pvec);

	   if (det < EPSILON)
		  return 0;

	   /* calculate distance from vert0 to ray origin */
	   SUB(tvec, orig, vert0);

	   /* calculate U parameter and test bounds */
	   *u = DOT(tvec, pvec);
	   if (*u < 0.0 || *u > det)
		  return 0;

	   /* prepare to test V parameter */
	   CROSS(qvec, tvec, edge1);

		/* calculate V parameter and test bounds */
	   *v = DOT(dir, qvec);
	   if (*v < 0.0 || *u + *v > det)
		  return 0;

	   /* calculate t, scale parameters, ray intersects triangle */
	   *t = DOT(edge2, qvec);
	   inv_det = 1.0 / det;
	   *t *= inv_det;
	   *u *= inv_det;
	   *v *= inv_det;

	   return 1;
	}



	/* code rewritten to do tests on the sign of the determinant */
	/* the division is at the end in the code                    */
	int intersect_triangle1(float orig[3], float dir[3],
				float vert0[3], float vert1[3], float vert2[3],
				float *t, float *u, float *v)
	{
	   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	   float det,inv_det;

	   /* find vectors for two edges sharing vert0 */
	   SUB(edge1, vert1, vert0);
	   SUB(edge2, vert2, vert0);

	   /* begin calculating determinant - also used to calculate U parameter */
	   CROSS(pvec, dir, edge2);

	   /* if determinant is near zero, ray lies in plane of triangle */
	   det = DOT(edge1, pvec);

	   if (det > EPSILON)
	   {
		  /* calculate distance from vert0 to ray origin */
		  SUB(tvec, orig, vert0);
	      
		  /* calculate U parameter and test bounds */
		  *u = DOT(tvec, pvec);
		  if (*u < 0.0 || *u > det)
		 return 0;
	      
		  /* prepare to test V parameter */
		  CROSS(qvec, tvec, edge1);
	      
		  /* calculate V parameter and test bounds */
		  *v = DOT(dir, qvec);
		  if (*v < 0.0 || *u + *v > det)
		 return 0;
	      
	   }
	   else if (det < -EPSILON)
	   {
		  /* calculate distance from vert0 to ray origin */
		  SUB(tvec, orig, vert0);
	      
		  /* calculate U parameter and test bounds */
		  *u = DOT(tvec, pvec);
	/*      printf("*u=%f\n",(float)*u); */
	/*      printf("det=%f\n",det); */
		  if (*u > 0.0 || *u < det)
		 return 0;
	      
		  /* prepare to test V parameter */
		  CROSS(qvec, tvec, edge1);
	      
		  /* calculate V parameter and test bounds */
		  *v = DOT(dir, qvec) ;
		  if (*v > 0.0 || *u + *v < det)
		 return 0;
	   }
	   else return 0;  /* ray is parallell to the plane of the triangle */


	   inv_det = 1.0 / det;

	   /* calculate t, ray intersects triangle */
	   *t = DOT(edge2, qvec) * inv_det;
	   (*u) *= inv_det;
	   (*v) *= inv_det;

	   return 1;
	}

	/* code rewritten to do tests on the sign of the determinant */
	/* the division is before the test of the sign of the det    */
	int intersect_triangle2(float orig[3], float dir[3],
				float vert0[3], float vert1[3], float vert2[3],
				float *t, float *u, float *v)
	{
	   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	   float det,inv_det;

	   /* find vectors for two edges sharing vert0 */
	   SUB(edge1, vert1, vert0);
	   SUB(edge2, vert2, vert0);

	   /* begin calculating determinant - also used to calculate U parameter */
	   CROSS(pvec, dir, edge2);

	   /* if determinant is near zero, ray lies in plane of triangle */
	   det = DOT(edge1, pvec);

	   /* calculate distance from vert0 to ray origin */
	   SUB(tvec, orig, vert0);
	   inv_det = 1.0 / det;
	   
	   if (det > EPSILON)
	   {
		  /* calculate U parameter and test bounds */
		  *u = DOT(tvec, pvec);
		  if (*u < 0.0 || *u > det)
		 return 0;
	      
		  /* prepare to test V parameter */
		  CROSS(qvec, tvec, edge1);
	      
		  /* calculate V parameter and test bounds */
		  *v = DOT(dir, qvec);
		  if (*v < 0.0 || *u + *v > det)
		 return 0;
	      
	   }
	   else if (det < -EPSILON)
	   {
		  /* calculate U parameter and test bounds */
		  *u = DOT(tvec, pvec);
		  if (*u > 0.0 || *u < det)
		 return 0;
	      
		  /* prepare to test V parameter */
		  CROSS(qvec, tvec, edge1);
	      
		  /* calculate V parameter and test bounds */
		  *v = DOT(dir, qvec) ;
		  if (*v > 0.0 || *u + *v < det)
		 return 0;
	   }
	   else return 0;  /* ray is parallell to the plane of the triangle */

	   /* calculate t, ray intersects triangle */
	   *t = DOT(edge2, qvec) * inv_det;
	   (*u) *= inv_det;
	   (*v) *= inv_det;

	   return 1;
	}

	/* code rewritten to do tests on the sign of the determinant */
	/* the division is before the test of the sign of the det    */
	/* and one CROSS has been moved out from the if-else if-else */
	int intersect_triangle3(float orig[3], float dir[3],
				float vert0[3], float vert1[3], float vert2[3],
				float *t, float *u, float *v)
	{
	   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	   float det,inv_det;

	   /* find vectors for two edges sharing vert0 */
	   SUB(edge1, vert1, vert0);
	   SUB(edge2, vert2, vert0);

	   /* begin calculating determinant - also used to calculate U parameter */
	   CROSS(pvec, dir, edge2);

	   /* if determinant is near zero, ray lies in plane of triangle */
	   det = DOT(edge1, pvec);

	   /* calculate distance from vert0 to ray origin */
	   SUB(tvec, orig, vert0);
	   inv_det = 1.0 / det;
	   
	   CROSS(qvec, tvec, edge1);
	      
	   if (det > EPSILON)
	   {
		  *u = DOT(tvec, pvec);
		  if (*u < 0.0 || *u > det)
		 return 0;
	            
		  /* calculate V parameter and test bounds */
		  *v = DOT(dir, qvec);
		  if (*v < 0.0 || *u + *v > det)
		 return 0;
	      
	   }
	   else if (det < -EPSILON)
	   {
		  /* calculate U parameter and test bounds */
		  *u = DOT(tvec, pvec);
		  if (*u > 0.0 || *u < det)
		 return 0;
	      
		  /* calculate V parameter and test bounds */
		  *v = DOT(dir, qvec) ;
		  if (*v > 0.0 || *u + *v < det)
		 return 0;
	   }
	   else return 0;  /* ray is parallell to the plane of the triangle */

	   *t = DOT(edge2, qvec) * inv_det;
	   (*u) *= inv_det;
	   (*v) *= inv_det;

	   return 1;
	}


#if 0
	bool Math::rayTraceTriangles(const Vector3 &start, const Vector3 &end, float *vertbuf, UShort *idxbuf, int num_idxs, float &f) {
		return false;
	}
#endif

AX_END_NAMESPACE
