/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CORE_CORETYPES_H
#define AX_CORE_CORETYPES_H

/*------------------------------------------------------------------------------
supported OS platform macro:
_WIN32 : Microsoft Windows 32 bits
    have such sub-define
    _WIN64 : Microsoft Windows 64 bits

MACOSX : Apple Mac OS X, we don't support old MacOS now
__linux__ : Linux
__FreeBSD__ : FreeBSD

supported C++ compiler macro:
_MSC_VER : Microsoft Visual C++ Compiler
__GNUG__ : GNU C++ Compiler, equivalent to testing (__GNUC__ && __cplusplus)
__ICC : Intel C++ Compiler
__MWERKS__ : Metrowerks C++ Compiler
__BORLANDC__: Borland C++ Compiler

support CPU(architecture) macro: some Compilers can produce code for more than one cpu
for:
__BORLANDC__
_MSC_VER -- VC
	_M_ALPHA : Alpha
	_M_IX86 : x86
	_M_IA64 : x86-ia64
	_M_MPPC : Power Macintosh platforms (no longer supported)
	_M_MRX000 : MIPS platforms (no longer supported)
	_M_PPC : PowerPC platforms (no longer supported)

for:
__BORLANDC__
__GNUG__
	__i386__ : x86-32
	__x86_64__  : x86-64
	__ppc__ : Power PC
	__axp__ : Alpha
------------------------------------------------------------------------------*/

#ifndef __cplusplus
#	error Engine requires C++ compilation (use a .cpp suffix)
#endif


// define OS_WIN and OS_UNIX and OS_UNIX
#if defined(_WIN32) || defined(_WIN64) || defined(_XBOX) || defined(__MINGW32__)
#   define OS_WIN       // windows system api
#else
#   define OS_UNIX      // unix/posix system api
#endif

#if defined(_XBOX) || defined(__PSX2_EE__) || defined(__GCN__)
#   define OS_CONSOLE   // game console
#endif

#ifndef AX_VERSION
#	define AX_VERSION "8.04"
#endif

// Define pseudo-keywords.
#ifndef IN
#	define IN
#endif

#ifndef OPTIONAL
#	define OPTIONAL
#endif

#ifndef OUT
#	define OUT
#endif

#define AX_NAMESPACE AXON

#ifdef AX_NAMESPACE
#define AX_BEGIN_NAMESPACE namespace AX_NAMESPACE {
#define AX_END_NAMESPACE }
#define AX_USE_NAMESPACE using namespace AX_NAMESPACE;
#else
#	define AX_NAMESPACE
#	define AX_BEGIN_NAMESPACE
#	define AX_END_NAMESPACE
#	define AX_USE_NAMESPACE
#endif

#if defined(_MSC_VER)
#   include "cc_msvc.h"
#elif defined(__GNUG__)
#   include "cc_gcc.h"
#elif
#   error do not support compiler
#endif


#if defined(_WIN32)
#	include "os_win.h"
#elif defined(__MACOSX__)
#	include "os_mac.h"
#elif defined(__linux__) || defined(__CYGWIN__)
#	include "os_linux.h"
#else
#	error Not supported platform
#endif

#if defined(_M_IX86) || defined(__i386__)
#	define AX_LITTLE_ENDIAN
#else
#	define AX_BIG_ENDIAN
#endif

#define FORMAT_ESCAPE '^'
#define IsColorString(p)	(p && *(p) == FORMAT_ESCAPE && *((p)+1) && *((p)+1) != FORMAT_ESCAPE)

#define C_COLOR_BLACK '0'
#define C_COLOR_RED '1'
#define C_COLOR_GREEN '2'
#define C_COLOR_YELLOW '3'
#define C_COLOR_BLUE '4'
#define C_COLOR_CYAN '5'
#define C_COLOR_MAGENTA '6'
#define C_COLOR_WHITE '7'
#define C_ColorIndex(c)	(((c) - '0') & 7)

#define S_COLOR_BLACK "^0"
#define S_COLOR_RED "^1"
#define S_COLOR_GREEN "^2"
#define S_COLOR_YELLOW "^3"
#define S_COLOR_BLUE "^4"
#define S_COLOR_CYAN "^5"
#define S_COLOR_MAGENTA "^6"
#define S_COLOR_WHITE "^7"

#ifndef _DEBUG
#	define AX_ASSERT(x)
#else
#	define AX_ASSERT(x) assert(x)
#endif

#define AX_RELEASE_ASSERT(x) if (!(x)) Errorf("%s: ASSURE \'%s\' FAILURE", __func__, #x);

#define AX_WRONGPLACE AX_ASSERT(0)

#define AX_MAKEFOURCC(ch0, ch1, ch2, ch3)															\
((unsigned int)(unsigned char)(ch0)			| ((unsigned int)(unsigned char)(ch1) << 8) |		\
((unsigned int)(unsigned char)(ch2) << 16)	| ((unsigned int)(unsigned char)(ch3) << 24))

#define AX_JOINT(x, y) x ## y

#define AX_WIDEN(x) AX_JOINT(L, x)

#define AX_MB_LEN_MAX 2 // multi-bytes char max length
#define AX_UTF8_LEN_MAX 4 // utf8 char max length

#define AX_BIT(x) (1<<x)

AX_BEGIN_NAMESPACE

#if 1
typedef signed char sbyte_t;
typedef unsigned char byte_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;
typedef unsigned long ulong_t;
typedef long long longlong_t;
typedef unsigned long long ulonglong_t;
#endif

class Handle
{
public:
	Handle() : m_data(0) {}
	Handle(void *d) : m_data(d) {}

	template <class T>
	T castTo() const { /*AX_STATIC_ASSERT(sizeof(T)<=sizeof(Handle));*/ return reinterpret_cast<T>(m_data); }

	void *toVoidStar() const { return m_data; }

	uintptr_t toInt() const { return reinterpret_cast<uintptr_t>(m_data); }

	bool operator==(const Handle &rhs) const { return m_data == rhs.m_data; }
	bool operator!() const { return !m_data; }
	operator bool() const { return bool(m_data); }

	void clear() { m_data = 0; }

private:
	void *m_data;
};
typedef Handle *phandle_t;
typedef const Handle *cphandle_t;

template <class T>
T handle_cast(const Handle & h)
{
	return h.castTo<T>();
}

template<typename T>
void SafeDelete(T*& p)
{
	if (p) {
		delete p;
		p = NULL;
	}
}

template<typename T>
void SafeDeleteArray(T*& p)
{
	if (p) {
		delete[] p;
		p = NULL;
	}
}

template<typename T>
void SafeFree(T*& p)
{
	if (p) {
		free(p);
		p = NULL;
	}
}

template<typename T>
void SafeDecRef(T*& p)
{
	if (p) {
		p->decref();
		p = NULL;
	}
}

template<typename T>
void SafeIncRef(T*& p)
{
	if (p) {
		p->incref();
	}
}

template<typename T>
void SafeRelease(T*& p)
{
	if (p) {
		p->release();
		p = NULL;
	}
}

template<typename T>
void SafeAddref(T*& p)
{
	if (p) {
		p->addref();
	}
}

template<typename T>
void SafeClearContainer(T &t)
{
	T::iterator it = t.begin();
	for (; it != t.end(); ++it) {
		SafeDelete(*it);
	}
}

template<typename T, size_t size>
size_t ArraySize(T (&)[size])
{
	return size;
}


template <typename T>
T ByteSwap(T val)
{
	size_t size = sizeof(T);

	if (size == 1) return val;

	T out;
	byte_t *inp = reinterpret_cast<byte_t*>(&val);
	byte_t *outp = reinterpret_cast<byte_t*>(&out);

	for (size_t i=0; i<size; i++) {
		outp[i] = inp[size-i-1];
	}

	return out;
}

#if !defined (AX_LITTLE_ENDIAN) && !defined (AX_BIG_ENDIAN)
#	error No AX_XXX_ENDIAN macro defined in your OS-specific kpOsDef.h!
#endif

#if defined(AX_LITTLE_ENDIAN)
template <typename T>
T LittleEndian(T val) { return val; }
#else
template <typename T>
T LittleEndian(T val) { return ByteSwap(val); }
#endif

AX_END_NAMESPACE

#endif // AX_CORE_CORETYPES_H
