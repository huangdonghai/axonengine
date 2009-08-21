/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "map_local.h"

namespace Axon { namespace Editor { namespace MapEdit {

	MapState::MapState()
	{
		terrainBrushSize = 1;
		terrainBrushSoftness = 0;
		terrainBrushStrength = 1;
		terrainCurLayerId = -1;

		followTerrain = true;
		treeFilename = "/speedtrees/default.spt";

		//		entityClass = "RigidBody";
		entityClass = "Game.Physics.Animated";
	}

}}}