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

AX_BEGIN_NAMESPACE

#define AX_CVARDECL(name, defaultstring, flags) \
	AX_API extern Cvar *name;
#include "cvardecls.h"
#undef AX_CVARDECL


#define AX_STATDECL(name, desc, autoreset) \
	AX_API extern int name;
#include "statdecls.h"
#undef AX_STATDECL

AX_API extern FontPtr g_defaultFont;
AX_API extern FontPtr g_consoleFont;
AX_API extern FontPtr g_miniFont;

AX_API extern RenderSystem *g_renderSystem;
AX_API extern RenderQueue *g_renderQueue;
AX_API extern RenderQueue *g_queues[2];
AX_API extern ShaderMacro g_shaderMacro;
AX_API extern Uniforms g_uniforms;

AX_API extern InputSystem *g_inputSystem;
AX_API extern SoundSystem *g_soundSystem;

AX_API extern ShaderManager *g_shaderManager;
AX_API extern TargetManager *g_targetManager;
AX_API extern QueryManager *g_queryManager;
AX_API extern PrimitiveManager *g_primitiveManager;

AX_API void axClientInit();
AX_API void axClientQuit();

AX_END_NAMESPACE

#undef AX_API

#endif // AX_RENDER_PUBLIC_H


