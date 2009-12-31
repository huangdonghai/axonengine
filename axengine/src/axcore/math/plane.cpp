/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

	Plane::Side Plane::side(const BoundingBox &bbox) const {
		int i;
		float dist1, dist2;
		int sides;
		static Vector3 corners[2];
#if 0
		for (i=0 ; i<3 ; i++) {
			if (*(&a + i) < 0) {
				corners[0][i] = bbox.low[i];
				corners[1][i] = bbox.high[i];
			} else {
				corners[1][i] = bbox.low[i];
				corners[0][i] = bbox.high[i];
			}
		}
#else
		// zsh hack地址，消除[]调用
		const float *L = &bbox.min.x;
		const float *H = &bbox.max.x;
		float *D0 = &corners[0].x;
		float *D1 = &corners[1].x;
		for (i=0 ; i<3 ; i++) {
			if (*(&a + i) < 0) {
				D0[i] = L[i];
				D1[i] = H[i];
			} else {
				D1[i] = L[i];
				D0[i] = H[i];
			}
		}
#endif
		dist1 = distance(corners[0]);
		dist2 = distance(corners[1]);

		sides = Invalid;
		if (dist1 >= 0)
			sides = Front;
		if (dist2 < 0)
			sides += Back;

		return (Side)sides;
	}

AX_END_NAMESPACE
