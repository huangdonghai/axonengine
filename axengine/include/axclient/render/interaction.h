/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_INTERACTION_H
#define AX_RENDER_INTERACTION_H

namespace Axon { namespace Render {

	struct QueuedLight;
	struct QueuedEntity;
	class Primitive;

	// sortkey is
	// opacity(1) matid(15) actorid(16) lightflags(32)
	struct Interaction {
		enum {
			MAX_LIGHTS = 4,
			MAX_TARGETS = 8
		};

		uint_t sortkey;

		QueuedEntity* qactor;
		Primitive* primitive;

		int numTargets;
		Target* targets[MAX_TARGETS];

		int resource;		// geometry resource
		Shader* shader;

		uint_t frameNum;

		bool instanced;

		// links
		Interaction* actorNext;
		Interaction* primNext;

		void calcSort(bool eyeInWater);
		void setupShader();
	};

}} // namespace Axon::Render

#endif // end guardian

