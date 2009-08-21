/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CLIENT_PUBLIC_H
#define AX_CLIENT_PUBLIC_H

#ifdef AXLOGIC_EXPORTS
#	define AX_API AX_DLL_EXPORT
#else
#	ifdef _LIB
#		define AX_API
#	else
#		define AX_API AX_DLL_IMPORT
#	endif
#endif


#include "userinput.h"

#include "network/all.h"
#include "physics/all.h"
#include "map/all.h"
#include "gfx/all.h"
#include "game/all.h"

namespace Axon {

#define AX_CVARDECL(name, defaultstring, flags) \
	AX_API extern Cvar* name;
#include "logiccvars.h"
#undef AX_CVARDECL


	typedef Game::GameSystem GameSystem;
	typedef Game::World GameWorld;
	typedef Game::Node GameNode;
	typedef Game::Entity GameEntity;

	AX_API extern GameSystem* g_gameSystem;

	AX_API extern void axLogicInit();
	AX_API extern void axLogicQuit();

} // namespace Axon

#undef AX_API

#endif // end guardian

