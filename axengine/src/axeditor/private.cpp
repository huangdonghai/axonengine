/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

namespace Axon {

	MapContext* g_mapContext = 0;
	GfxContext* g_gfxContext = 0;

	void axEditorInit() {
		g_mapContext = new MapContext();
	}

	void axEditorQuit() {
		SafeDelete(g_mapContext);
	}

} // namespace Axon
