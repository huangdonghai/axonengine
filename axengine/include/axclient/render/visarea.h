/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_RENDER_VISAREA_H
#define AX_RENDER_VISAREA_H

AX_BEGIN_NAMESPACE

#if 0
struct ConvexSide {
	Vector4 plane;
	bool isSolid;
};

typedef Sequence<ConvexSide> ConvexSideSeq;

struct Convex {
	int numPlanes;
	ConvexSideSeq sides;
};
typedef Sequence<Convex> ConvexSeq;

class VisArea : public Actor {
public:
	BoundingBox m_linkedBbox;
	float m_height;
	Sequence<Vector3>	points;

	Portal *m_portals;
	bool inSun;
	IntSeq brushes;
	Primitives primitives;
};
#endif

AX_END_NAMESPACE

#endif // end guardian

