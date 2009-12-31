/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#ifndef AX_CORE_SIMD_H
#define AX_CORE_SIMD_H

AX_BEGIN_NAMESPACE


	class AX_API Simd {
	public:
		static void initialize();
		virtual void computeTangentSpace(Vertex *verts, int numVerts, const ushort_t *indexes, int numIndexes);
		virtual void computeTangentSpaceSlow(Vertex *verts, int numVerts, const ushort_t *indexes, int numIndexes);
		virtual void findHeightMinMax(const ushort_t *data, int stride, const Rect &rect, ushort_t *minh, ushort_t *maxh);
	};

AX_END_NAMESPACE

#endif // end guardian

