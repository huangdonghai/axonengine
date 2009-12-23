/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_EDITOR_PUBLIC_H
#define AX_EDITOR_PUBLIC_H

#ifdef AXEDITOR_EXPORTS
#	define AX_API AX_DLL_EXPORT
#else
#	ifdef _LIB
#		define AX_API
#	else
#		define AX_API AX_DLL_IMPORT
#	endif
#endif


#include "types.h"
#include "gizmo.h"
#include "actor.h"
#include "action.h"
#include "history.h"
#include "tools.h"
#include "view.h"
#include "state.h"
#include "context.h"

#include "map/all.h"
#include "gfxedit/all.h"

// map editing

namespace Axon {

	AX_API extern MapContext* g_mapContext;

	AX_API void axEditorInit();
	AX_API void axEditorQuit();

} // namespace Axon

#undef AX_API

#endif // end guardian

