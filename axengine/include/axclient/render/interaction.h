/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_INTERACTION_H
#define AX_RENDER_INTERACTION_H

AX_BEGIN_NAMESPACE

#if 0
struct QueuedLight;
struct QueuedEntity;
#endif

class Primitive;

// sortkey is
// opacity(1) matid(15) actorid(16) lightflags(32)
struct Interaction {
	enum {
		MAX_LIGHTS = 4,
	};

	uint_t sortkey;

	RenderEntity *entity;
	Primitive *primitive;

	uint_t frameNum;

	bool instanced;

	// links
	Interaction *actorNext;
	Interaction *primNext;

	void calcSort(bool eyeInWater);
};

AX_END_NAMESPACE

#endif // end guardian

