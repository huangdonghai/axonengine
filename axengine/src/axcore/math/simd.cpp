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

	void Simd::computeTangentSpace(MeshVertex *verts, int numVerts, const ushort_t *indexes, int numIndexes) {
		bool *used = (bool *)Alloca16(numVerts * sizeof(used[0]));
		memset(used, 0, numVerts * sizeof(used[0]));

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

			d0[0] = b->xyz[0] - a->xyz[0];
			d0[1] = b->xyz[1] - a->xyz[1];
			d0[2] = b->xyz[2] - a->xyz[2];
			d0[3] = b->st[0] - a->st[0];
			d0[4] = b->st[1] - a->st[1];

			d1[0] = c->xyz[0] - a->xyz[0];
			d1[1] = c->xyz[1] - a->xyz[1];
			d1[2] = c->xyz[2] - a->xyz[2];
			d1[3] = c->st[0] - a->st[0];
			d1[4] = c->st[1] - a->st[1];

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
				a->normal += n;
				a->tangent += t0;
				a->binormal += t1;
			} else {
				a->normal = n;
				a->tangent = t0;
				a->binormal = t1;
				used[v0] = true;
			}

			if (used[v1]) {
				b->normal += n;
				b->tangent += t0;
				b->binormal += t1;
			} else {
				b->normal = n;
				b->tangent = t0;
				b->binormal = t1;
				used[v1] = true;
			}

			if (used[v2]) {
				c->normal += n;
				c->tangent += t0;
				c->binormal += t1;
			} else {
				c->normal = n;
				c->tangent = t0;
				c->binormal = t1;
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

			d0[0] = b->xyz[0] - a->xyz[0];
			d0[1] = b->xyz[1] - a->xyz[1];
			d0[2] = b->xyz[2] - a->xyz[2];
			d0[3] = b->st[0] - a->st[0];
			d0[4] = b->st[1] - a->st[1];

			d1[0] = c->xyz[0] - a->xyz[0];
			d1[1] = c->xyz[1] - a->xyz[1];
			d1[2] = c->xyz[2] - a->xyz[2];
			d1[3] = c->st[0] - a->st[0];
			d1[4] = c->st[1] - a->st[1];

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
				a->tangent += t0;
				a->binormal += t1;
			} else {
				a->tangent = t0;
				a->binormal = t1;
				used[v0] = true;
			}

			if (used[v1]) {
				b->tangent += t0;
				b->binormal += t1;
			} else {
				b->tangent = t0;
				b->binormal = t1;
				used[v1] = true;
			}

			if (used[v2]) {
				c->tangent += t0;
				c->binormal += t1;
			} else {
				c->tangent = t0;
				c->binormal = t1;
				used[v2] = true;
			}
		}

		// pass two, fix tangent
		for (int i = 0; i < numVerts; i++) {
			if (!used[i])
				continue;

			MeshVertex *v = verts + i;
			Vector3 b = v->normal ^ v->tangent;

			if ((b | v->binormal) < 0)
				b = -b;

			Vector3 t = b ^ v->normal;
			if ((t | v->tangent) < 0)
				t = -t;

			v->tangent = t.getNormalized();
			v->binormal = b.getNormalized();
		}
	}

AX_END_NAMESPACE

