/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_GAME_PUBLIC_H
#define AX_GAME_PUBLIC_H

#if 0
#ifdef _LIB
#	undef AX_LOGIC_API
#	define AX_LOGIC_API
#else
#	ifndef AX_LOGIC_API
#		define AX_DLL_IMPORT
#	endif
#endif
#endif

AX_BEGIN_NAMESPACE
AX_END_NAMESPACE

#include "game_object.h"
#include "game_actor.h"
#include "landscape.h"
#include "game_world.h"
#include "game_system.h"

#endif // end guardian

