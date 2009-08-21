/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "private.h"

namespace Axon {

	AX_DECLARE_MODULE(axopengl);
	AX_DECLARE_MODULE(axdirect3d9);

	using namespace Axon::Render;
//	using namespace Axon::PARTICLESYSTEM;

	// console variable
#define AX_CVARDECL(name, defaultstring, flags) \
	Cvar* name;
#include <axclient/cvardecls.h>
#undef AX_CVARDECL

	// statistic
#define AX_STATDECL(name, desc, autoreset)  int name;
#include "axclient/statdecls.h"
#undef AX_STATDECL

	// public object
	Render::System* g_renderSystem;
	Render::Queue* g_renderQueue;
	Render::Queue* g_queues[2];
	Render::ShaderMacro			g_shaderMacro;
	Render::Uniforms			g_uniforms;
	Render::IDriver* g_renderDriver;

	Render::TargetManager* g_targetManager;
	Render::ShaderManager* g_shaderManager;
	Render::PrimitiveManager* g_primitiveManager;

	Render::QueryManager* g_queryManager;

	InputSystem* g_inputSystem;
	SoundSystem* g_soundSystem;

	FontPtr						g_defaultFont;
	FontPtr						g_consoleFont;
	FontPtr						g_miniFont;

	// only visible in this module
	FontFactory* g_fontFactory;
	MaterialFactory* gMaterialFactory;

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
	TreeManager* g_treeManager;
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

	// xinput
//	Xinput* gXinput;

	void axClientInit() {

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


#if 1 // _LIB
		AX_REGISTER_MODULE(axopengl);
		AX_REGISTER_MODULE(axdirect3d9);
#endif

		g_renderSystem = new Render::System;
		g_renderSystem->initialize();

		gMaterialFactory = new MaterialFactory;
		g_assetManager->registerType(RenderMaterial::AssetType, gMaterialFactory);

		g_fontFactory = new FontFactory;
		g_assetManager->registerType(Asset::kFont, g_fontFactory);
		g_fontFactory->initialize();

		g_defaultFont = FindAsset_<Font>("fonts/default", Font::makeSize(14,14));
		g_consoleFont = FindAsset_<Font>("fonts/console", Font::makeSize(14,14));
		g_miniFont = FindAsset_<Font>("fonts/console", Font::makeSize(11,11));

		g_renderDriver->postInit();

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
		g_treeManager = new TreeManager();
		g_renderSystem->addEntityManager(g_treeManager);
#endif

		g_inputSystem = new InputSystem();
		g_inputSystem->initialize();

		g_soundSystem = new SoundSystem();

//		g_shaderManager->applyShaderCache();
	}

	void axClientQuit() {
//		g_shaderManager->saveShaderCache();

		SafeDelete(g_soundSystem);

		g_inputSystem->finalize();
		SafeDelete(g_inputSystem);

		g_defaultFont.clear();
		g_consoleFont.clear();
		g_miniFont.clear();

#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40
		g_renderSystem->removeEntityManager(g_treeManager);
		SafeDelete(g_treeManager);
#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

		g_fontFactory->finalize();
		g_assetManager->removeType(Asset::kFont);
		SafeDelete(g_fontFactory);

		g_assetManager->removeType(RenderMaterial::AssetType);
		SafeDelete(gMaterialFactory);

		g_renderSystem->finalize();
		SafeDelete(g_renderSystem);

		g_renderQueue->finalize();
		SafeDelete(g_renderQueue);
	}

} // namespace Axon
