/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

AX_BEGIN_NAMESPACE

AX_DECLARE_MODULE(axopengl);
AX_DECLARE_MODULE(axdirect3d9);

// console variable
#define AX_CVARDECL(name, defaultstring, flags) \
	Cvar name(#name, defaultstring, flags);
	#include <axclient/cvardecls.h>
#undef AX_CVARDECL

// statistic
#define AX_STATDECL(name, desc, autoreset)  Stat name("Client", desc, autoreset, desc);
	#include "axclient/statdecls.h"
#undef AX_STATDECL

// public object
RenderSystem *g_renderSystem;

#if 0
RenderTargetManager *g_targetManager;
ShaderManager *g_shaderManager;
PrimitiveManager *g_primitiveManager;
QueryManager *g_queryManager;
#endif

InputSystem *g_inputSystem;
SoundSystem *g_soundSystem;

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	TreeManager *g_treeManager;
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40


void axClientInit()
{
#if 0
	// init cvar
#define AX_CVARDECL(name, defaultstring, flags) \
	name = g_cvarSystem->createCvar(#name, defaultstring, flags);
	#include <axclient/cvardecls.h>
#undef AX_CVARDECL

	// init statistic
#define AX_STATDECL(name, desc, autoreset) \
	name = g_statistic->getIndex(Statistic::RenderDrvGroup, desc, autoreset);
	#include "axclient/statdecls.h"
#undef AX_STATDECL
#endif

#if 1 // _LIB
//	AX_REGISTER_MODULE(axopengl);
	AX_REGISTER_MODULE(axdirect3d9);
#endif

	g_renderSystem = new RenderSystem;
	g_renderSystem->initialize();


#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	g_treeManager = new TreeManager();
	g_renderSystem->addEntityManager(g_treeManager);
#endif

	g_inputSystem = new InputSystem();
	g_inputSystem->initialize();

	g_soundSystem = new SoundSystem();
}

void axClientQuit()
{
	SafeDelete(g_soundSystem);

	g_inputSystem->finalize();
	SafeDelete(g_inputSystem);


#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	g_renderSystem->removeEntityManager(g_treeManager);
	SafeDelete(g_treeManager);
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

	g_renderSystem->finalize();
	SafeDelete(g_renderSystem);

	g_renderFrame->finalize();
	SafeDelete(g_renderFrame);
}

AX_END_NAMESPACE
