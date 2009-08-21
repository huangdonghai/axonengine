/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_CORE_STDINC_H
#define AX_CORE_STDINC_H

#if _MSC_VER >= 1400
#	define _CRT_SECURE_NO_DEPRECATE
#	define _HAS_ITERATOR_DEBUGGING 0
#	define _SECURE_SCL 0
#endif

#if 0
#if _MSC_VER >= 1400
#	define _CRT_SECURE_NO_WARNINGS
#	define _SCL_SECURE_NO_WARNINGS
#	define _CRT_SECURE_NO_DEPRECATE
#	define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#	define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#endif
#endif

// c header include file
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>			// tolower and toupper for Linux
#include <assert.h>
#include <malloc.h>			// _alloca

// c++ header file
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <memory>
#include <limits>
#include <sstream>

// tr1
#include <unordered_map>
#include <unordered_set>
#include <memory>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#endif // AX_CORE_STDINC_H
