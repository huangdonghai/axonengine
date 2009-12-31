/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "../private.h"

AX_BEGIN_NAMESPACE

	PhysicsSystem *g_physicsSystem;
	HavokPackageManager *g_havokPackageManager;


	void axPhysInit() {
		g_physicsSystem = new PhysicsSystem;
		g_physicsSystem->initialize();

		g_havokPackageManager = new HavokPackageManager;
	}

	void axPhysQuit() {
		SafeDelete(g_havokPackageManager);

		g_physicsSystem->finalize();
		SafeDelete(g_physicsSystem);
	}

AX_END_NAMESPACE

