/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"


#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

#define s_curve(t) (t * t * (3. - 2. * t))

#define lerp(t, a, b) (a + t * (b - a))

namespace {
#if 0
	inline float s_curve(float t) {
		return t * t * (3.f - 2.f * t);
	}

	inline float lerp(float t, float a, float b) {
		return a + t * (b - a);
	}
#endif
	static void
	normalize2(float v[2]) {
		float s;

		s = sqrt(v[0] * v[0] + v[1] * v[1]);
		v[0] = v[0] / s;
		v[1] = v[1] / s;
	}

	static void
	normalize3(float v[3]) {
		float s;

		s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		v[0] = v[0] / s;
		v[1] = v[1] / s;
		v[2] = v[2] / s;
	}
}

AX_BEGIN_NAMESPACE

PerlinNoise::PerlinNoise(uint_t seed /* = 0 */) {
	m_holdrand = seed;

	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((xrand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((xrand() % (B + B)) - B) / B;
//			normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((xrand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = xrand() % B];
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

inline int PerlinNoise::xrand() {
	return(((m_holdrand = m_holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

float PerlinNoise::noise(float arg) {
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];
	return lerp(sx, u, v);
}

float PerlinNoise::noise2(float x, float y) {
	float vec[2] = { x, y };
int 			bx0, bx1, by0, by1, b00, b10, b01, b11;
	float 			rx0, rx1, ry0, ry1, sx, sy, a, b, t, u, v;
	const float *q;
	register int i, j;

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

float PerlinNoise:: noise3( float x, float y, float z) {
	float vec[3] = { x, y, z };
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
	const float *q;
	register int i, j;

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

float PerlinNoise::noise2(const Vector2 &vec, int octaves, float persistence) {
	octaves += 1;

	float amplitude = 1.f;
	float total_amp = 0;

	float ret = 0.f;
	Vector2 v = vec;
	for (int i=0; i<octaves; i++) {
		ret += noise2(v.x, v.y) * amplitude;
		total_amp += amplitude;
		amplitude *= persistence;
		v *= 2.0f;
	}

	return ret / total_amp;
}

AX_END_NAMESPACE

