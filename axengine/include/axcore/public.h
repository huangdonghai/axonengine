/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_CORE_PUBLIC_H
#define AX_CORE_PUBLIC_H

#ifdef AXCORE_EXPORTS
#	define AX_API AX_DLL_EXPORT
#else
#	ifdef _LIB
#		define AX_API
#	else
#		define AX_API AX_DLL_IMPORT
#	endif
#endif

#include "coretypes.h"
#include "./thread/thread.h"
#include "./singleton/memory.h"
#include "./utility/string.h"
#include "./inputoutput/xml.h"
#include "./utility/osutil.h"
#include "./template/template.h"
#include "./utility/fixedstring.h"
#include "./math/math.h"
#include "./utility/pathutil.h"
#include "./utility/observer.h"
#include "./utility/calendar.h"
#include "./utility/uuid.h"
#include "./utility/refobject.h"
#include "./inputoutput/image.h"
#include "./singleton/classfactory.h"
#include "./singleton/scriptsystem.h"
#include "./singleton/filesystem.h"
#include "./singleton/configfile.h"
#include "./singleton/cmdsystem.h"
#include "./singleton/cvarsystem.h"
#include "./singleton/statistic.h"
#include "./singleton/assetmanager.h"
#include "./singleton/coresystem.h"
#include "./inputoutput/materialfile.h"

AX_BEGIN_NAMESPACE

	// global object
	AX_API extern FileSystem* g_fileSystem;
	AX_API extern CmdSystem* g_cmdSystem;
	AX_API extern CvarSystem* g_cvarSystem;
	AX_API extern ConfigFile* g_systemConfig;
	AX_API extern System* g_system;
	AX_API extern ClassFactory* g_classFactory;
	AX_API extern ScriptSystem* g_scriptSystem;
	AX_API extern Statistic* g_statistic;
	AX_API extern Simd* g_simd;

	// global cvar
#define AX_CVARDECL(name, defaultstring, flags) \
	extern Cvar* name;
#include "cvardecls.h"
#undef AX_CVARDECL

//	AX_API extern AssetManager* g_assetManager;

	AX_API void axCoreInit(int argc, char* argv[]);
	AX_API void axCoreQuit();

AX_END_NAMESPACE

#undef AX_API

#endif // AX_CORE_PUBLIC_H
