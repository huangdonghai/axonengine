/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/




#include "private.h"


namespace Axon {

#define AX_CVARDECL(name, defaultstring, flags) \
	Cvar* name;
#include <axlogic/logiccvars.h>
#undef AX_CVARDECL

	void axPhysInit();
	void axPhysQuit();

	GameSystem* g_gameSystem;

	void axLogicInit() {
		// init cvar
#define AX_CVARDECL(name, defaultstring, flags) \
	name = g_cvarSystem->createCvar(#name, defaultstring, flags);
#include <axlogic/logiccvars.h>
#undef AX_CVARDECL

		axPhysInit();

		g_gameSystem = new GameSystem();

		AX_REGISTER_CLASS(Game::GameEntity);
		AX_REGISTER_CLASS(Game::GameLight);
		AX_REGISTER_CLASS(Game::GameSound);
		AX_REGISTER_CLASS(Game::RigidBody);
		AX_REGISTER_CLASS(Game::Animated);
		AX_REGISTER_CLASS(Game::Articulated);
		AX_REGISTER_CLASS(Game::Player);
	}

	void axLogicQuit() {
		SafeDelete(g_gameSystem);

		axPhysQuit();
	}

} // namespace Axon

