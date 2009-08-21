/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_ENGINE_PRIVATE_H
#define AX_ENGINE_PRIVATE_H

#include <axcore/public.h>

namespace Axon {
	void DetectCpuInfo(CpuInfo& cpu_info );

	extern FILE* g_logFile;

}
#endif // AX_ENGINE_PRIVATE_H
