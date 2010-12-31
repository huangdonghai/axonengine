/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

void Simd::initialize() {
	g_simd = new Simd();
}

void Simd::computeTangentSpace(MeshVertex *verts, int numVerts, const ushort_t *indexes, int numIndexes)
{
	bool *used = (bool *)Alloca16(numVerts * sizeof(used[0]));
	memset(used, 0, numVerts * sizeof(used[0]));

	for (int i = 0; i < numIndexes; i += 3) {
		MeshVertex *a, *b, *c;
		Vector3 a_binormal, b_binormal, c_binormal;
		unsigned long signBit;
		float d0[5], d1[5], f, area;
		Vector3 n, t0, t1;

		int v0 = indexes[i + 0];
		int v1 = indexes[i + 1];
		int v2 = indexes[i + 2];

		a = verts + v0;
		b = verts + v1;
		c = verts + v2;

		d0[0] = b->position[0] - a->position[0];
		d0[1] = b->position[1] - a->position[1];
		d0[2] = b->position[2] - a->position[2];
		d0[3] = b->streamTc[0] - a->streamTc[0];
		d0[4] = b->streamTc[1] - a->streamTc[1];

		d1[0] = c->position[0] - a->position[0];
		d1[1] = c->position[1] - a->position[1];
		d1[2] = c->position[2] - a->position[2];
		d1[3] = c->streamTc[0] - a->streamTc[0];
		d1[4] = c->streamTc[1] - a->streamTc[1];

		// normal
		n[0] = d1[1] * d0[2] - d1[2] * d0[1];
		n[1] = d1[2] * d0[0] - d1[0] * d0[2];
		n[2] = d1[0] * d0[1] - d1[1] * d0[0];

		f = Math::rsqrt(n.x * n.x + n.y * n.y + n.z * n.z);

		n.x *= f;
		n.y *= f;
		n.z *= f;

		// area sign bit
		area = d0[3] * d1[4] - d0[4] * d1[3];
		signBit = (*(unsigned long *)&area) & (1 << 31);

		// first tangent
		t0[0] = d0[0] * d1[4] - d0[4] * d1[0];
		t0[1] = d0[1] * d1[4] - d0[4] * d1[1];
		t0[2] = d0[2] * d1[4] - d0[4] * d1[2];

		f = Math::rsqrt(t0.x * t0.x + t0.y * t0.y + t0.z * t0.z);
		*(unsigned long *)&f ^= signBit;

		t0.x *= f;
		t0.y *= f;
		t0.z *= f;

		// second tangent
		t1[0] = d0[3] * d1[0] - d0[0] * d1[3];
		t1[1] = d0[3] * d1[1] - d0[1] * d1[3];
		t1[2] = d0[3] * d1[2] - d0[2] * d1[3];

		f = Math::rsqrt(t1.x * t1.x + t1.y * t1.y + t1.z * t1.z);
		*(unsigned long *)&f ^= signBit;

		t1.x *= f;
		t1.y *= f;
		t1.z *= f;

		if (used[v0]) {
			a->normal.xyz() += n;
			a->tangent.xyz() += t0;
		} else {
			a->normal.xyz() = n;
			a->tangent.xyz() = t0;
			used[v0] = true;
		}

		if (used[v1]) {
			b->normal.xyz() += n;
			b->tangent.xyz() += t0;
		} else {
			b->normal.xyz() = n;
			b->tangent.xyz() = t0;
			used[v1] = true;
		}

		if (used[v2]) {
			c->normal.xyz() += n;
			c->tangent.xyz() += t0;
		} else {
			c->normal.xyz() = n;
			c->tangent.xyz() = t0;
			used[v2] = true;
		}
	}
}

void Simd::findHeightMinMax(const ushort_t *data, int stride, const Rect &rect, ushort_t *minh, ushort_t *maxh) {

}

void Simd::computeTangentSpaceSlow( MeshVertex *verts, int numVerts, const ushort_t *indexes, int numIndexes )
{
	bool *used = (bool *)Alloca16(numVerts * sizeof(used[0]));
	memset(used, 0, numVerts * sizeof(used[0]));

	// pass one
	for (int i = 0; i < numIndexes; i += 3) {
		MeshVertex *a, *b, *c;
		unsigned long signBit;
		float d0[5], d1[5], f, area;
		Vector3 n, t0, t1;

		int v0 = indexes[i + 0];
		int v1 = indexes[i + 1];
		int v2 = indexes[i + 2];

		a = verts + v0;
		b = verts + v1;
		c = verts + v2;

		d0[0] = b->position[0] - a->position[0];
		d0[1] = b->position[1] - a->position[1];
		d0[2] = b->position[2] - a->position[2];
		d0[3] = b->streamTc[0] - a->streamTc[0];
		d0[4] = b->streamTc[1] - a->streamTc[1];

		d1[0] = c->position[0] - a->position[0];
		d1[1] = c->position[1] - a->position[1];
		d1[2] = c->position[2] - a->position[2];
		d1[3] = c->streamTc[0] - a->streamTc[0];
		d1[4] = c->streamTc[1] - a->streamTc[1];

		// normal
		n[0] = d1[1] * d0[2] - d1[2] * d0[1];
		n[1] = d1[2] * d0[0] - d1[0] * d0[2];
		n[2] = d1[0] * d0[1] - d1[1] * d0[0];

		f = Math::rsqrt(n.x * n.x + n.y * n.y + n.z * n.z);

		n.x *= f;
		n.y *= f;
		n.z *= f;

		// area sign bit
		area = d0[3] * d1[4] - d0[4] * d1[3];
		signBit = (*(unsigned long *)&area) & (1 << 31);

		// first tangent
		t0[0] = d0[0] * d1[4] - d0[4] * d1[0];
		t0[1] = d0[1] * d1[4] - d0[4] * d1[1];
		t0[2] = d0[2] * d1[4] - d0[4] * d1[2];

		f = Math::rsqrt(t0.x * t0.x + t0.y * t0.y + t0.z * t0.z);
		*(unsigned long *)&f ^= signBit;

		t0.x *= f;
		t0.y *= f;
		t0.z *= f;

		// second tangent
		t1[0] = d0[3] * d1[0] - d0[0] * d1[3];
		t1[1] = d0[3] * d1[1] - d0[1] * d1[3];
		t1[2] = d0[3] * d1[2] - d0[2] * d1[3];

		f = Math::rsqrt(t1.x * t1.x + t1.y * t1.y + t1.z * t1.z);
		*(unsigned long *)&f ^= signBit;

		t1.x *= f;
		t1.y *= f;
		t1.z *= f;

		if (used[v0]) {
			a->tangent.xyz() += t0;
		} else {
			a->tangent.xyz() = t0;
			used[v0] = true;
		}

		if (used[v1]) {
			b->tangent.xyz() += t0;
		} else {
			b->tangent.xyz() = t0;
			used[v1] = true;
		}

		if (used[v2]) {
			c->tangent.xyz() += t0;
		} else {
			c->tangent.xyz() = t0;
			used[v2] = true;
		}
	}

	// pass two, fix tangent
	for (int i = 0; i < numVerts; i++) {
		if (!used[i])
			continue;

		MeshVertex *v = verts + i;
		Vector3 b = v->normal.xyz() ^ v->tangent.xyz();

		Vector3 t = b ^ v->normal.xyz();

		v->tangent.xyz() = t.getNormalized();
	}
}

AX_END_NAMESPACE

