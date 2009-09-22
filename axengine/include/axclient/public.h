/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_RENDER_PUBLIC_H
#define AX_RENDER_PUBLIC_H

#ifdef AXCLIENT_EXPORTS
#	define AX_API AX_DLL_EXPORT
#else
#	ifdef _LIB
#		define AX_API
#	else
#		define AX_API AX_DLL_IMPORT
#	endif
#endif

#include "render/all.h"
#include "input/all.h"
#include "speedtree/all.h"
#include "sound/all.h"

namespace Axon {

	typedef Render::Entity RenderEntity;
	typedef Render::Camera RenderCamera;
	typedef Render::System RenderSystem;
	typedef Render::World RenderWorld;
	typedef Render::Light RenderLight;
	typedef Render::Primitive RenderPrim;
	typedef Render::Line RenderLine;
	typedef Render::Mesh RenderMesh;
	typedef Render::Text RenderText;
	typedef Render::Chunk RenderChunk;
	typedef Render::Primitives RenderPrims;

	using Render::Material;
	using Render::MaterialPtr;
	using Render::Texture;
	using Render::TexturePtr;


#define AX_CVARDECL(name, defaultstring, flags) \
	AX_API extern Cvar* name;
#include "cvardecls.h"
#undef AX_CVARDECL


#define AX_STATDECL(name, desc, autoreset) \
	AX_API extern int name;
#include "statdecls.h"
#undef AX_STATDECL

	AX_API extern Render::FontPtr g_defaultFont;
	AX_API extern Render::FontPtr g_consoleFont;
	AX_API extern Render::FontPtr g_miniFont;

	AX_API extern Render::System* g_renderSystem;
	AX_API extern Render::Queue* g_renderQueue;
	AX_API extern Render::Queue* g_queues[2];
	AX_API extern Render::ShaderMacro g_shaderMacro;
	AX_API extern Render::Uniforms g_uniforms;

	AX_API extern InputSystem* g_inputSystem;
	AX_API extern SoundSystem* g_soundSystem;

	AX_API extern Render::ShaderManager* g_shaderManager;
	AX_API extern Render::TargetManager* g_targetManager;
	AX_API extern Render::QueryManager* g_queryManager;
	AX_API extern Render::PrimitiveManager* g_primitiveManager;
	AX_API extern Render::TextureManager* g_textureManager;

	AX_API void axClientInit();
	AX_API void axClientQuit();

} // namespace Axon

#undef AX_API

#endif // AX_RENDER_PUBLIC_H


