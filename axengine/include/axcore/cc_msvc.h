/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CC_VC_H
#define AX_CC_VC_H

#if defined(_MSC_VER)
#	pragma warning(disable : 4786)	// identifier was truncated to 256 characters in the debug information
#	pragma warning(disable : 4244)	// conversion from 'type1' to 'type2', possible loss of data
#	pragma warning(disable : 4284)	// return type for 'identifier::operator ->' is not a UDT or reference to a UDT. Will produce errors if applied using infix notation
#	pragma warning(disable : 4251)	// 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#	pragma warning(disable : 4275)	// non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'

#endif

#if _MSC_VER == 1400
#	pragma warning(disable : 4819)	// The file contains a character that cannot be represented in the current code page (number). Save the file in Unicode format to prevent data loss.
#endif

AX_BEGIN_NAMESPACE

typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef unsigned __int8 uint8_t;

AX_END_NAMESPACE

#define STDCALL     __stdcall

#ifndef CDECL
#   define CDECL __cdecl
#endif

#ifndef FASTCALL
#	define FASTCALL __fastcall
#endif

#define AX_DLL_IMPORT __declspec(dllimport)
#define AX_DLL_EXPORT __declspec(dllexport)

#define Alloca _alloca
#define Alloca16(x)					((void *)((((uintptr_t)_alloca((x)+15)) + 15) & ~15))

#define __AX_ASSUME __assume

#endif // AX_CC_VC_H
