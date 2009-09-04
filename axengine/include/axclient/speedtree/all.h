/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CLIENT_SPEEDTREE_ALL_H
#define AX_CLIENT_SPEEDTREE_ALL_H

#define AX_CONFIG_OPTION_USE_SPEEDTREE_40 1
#ifdef AX_CONFIG_OPTION_USE_SPEEDTREE_40

#include "treeactor.h"
#include "treeasset.h"
#include "treemanager.h"

namespace Axon {
	typedef Render::TreeActor RenderTree;
} // namespace Axon

#endif // AX_CONFIG_OPTION_USE_SPEEDTREE_40

#endif // end guardian

