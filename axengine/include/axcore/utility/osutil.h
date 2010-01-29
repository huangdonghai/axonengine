/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_OS_UTIL_H
#define AX_OS_UTIL_H

AX_BEGIN_NAMESPACE

struct FileInfo;
typedef Sequence<FileInfo> FileInfoSeq;

AX_API void CDECL Errorf(const char *fmt, ...);
AX_API void CDECL Debugf(const char *fmt, ...);
AX_API void CDECL Printf(const char *fmt, ...);

struct FilterInfo;
class Filter {
	std::vector<FilterInfo*>	mfilters;
public:
	Filter(const String &filterStr);
	~Filter();

	bool In(const String &name) const;
};


// for unstandardized C API wrapping
#define Strequ(s1, s2)	(strcmp(s1, s2) == 0)
#define Striequ(s1, s2)	(StringUtil::stricmp(s1, s2) == 0)
#define Strnequ(s1, s2, n)	(strncmp(s1, s2, n) == 0)


struct AX_API OsUtil {
#if 0
	static uint_t milliseconds();
	static ulonglong_t microseconds();
#endif
	static double getTime();
	static String getClipboardString();
	// DLL
	static handle_t loadDll(const String &name);
	static handle_t loadSysDll(const String &name);
	static void *getProcEntry(handle_t handle, const String &name);
	static bool freeDll(handle_t handle);

	// Network address or host name
	// get primary adapter's mac address(physics address)
	static bool getMacAddress(byte_t address[6]);
	static String getHostName();

	static bool mkdir(const char *dirname);
	static String getworkpath();

	static int getCpuUsage();
	static int getMemoryUsage();

	static void sleep(float seconds);
	static int getScreenWidth();
	static int getScreenHeight();

	static int atomicIncrement(int &i32Value)
	{
		return InterlockedIncrement((LONG*)&i32Value);
	}

	static int atomicDecrement(int &i32Value)
	{
		return InterlockedDecrement((LONG*)&i32Value);
	}

	static uint_t atomicIncrement(uint_t &ui32Value)
	{
		return InterlockedIncrement((LONG*)&ui32Value);
	}

	static uint_t atomicDecrement(uint_t &ui32Value)
	{
		AX_ASSERT(ui32Value > 0);
		return InterlockedDecrement((LONG*)&ui32Value);
	}
};

AX_END_NAMESPACE

#endif // AX_OS_UTIL_H
