/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_OS_WIN_H
#define AX_OS_WIN_H

#define OS_PATH_SEP '\\'
#define OS_PATH_SEP_STRING "\\"
#define OS_LINE_SEP "\r\n"

#define __func__ __FUNCTION__

#ifdef _DEBUG
#	define AX_BUILD "win32_x86_dbg"
#else
#	define AX_BUILD "win32_x86_rls"
#endif

#endif // AX_OS_WIN_H
