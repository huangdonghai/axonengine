/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

namespace Axon {

	PhysicsSystem* g_physicsSystem;
	PhysicsPackageManager* g_physicsPackageManager;


	void axPhysInit() {
		g_physicsSystem = new PhysicsSystem;
		g_physicsSystem->initialize();

		g_physicsPackageManager = new PhysicsPackageManager;
	}

	void axPhysQuit() {
		SafeDelete(g_physicsPackageManager);

		g_physicsSystem->finalize();
		SafeDelete(g_physicsSystem);
	}

} // namespace Axon

