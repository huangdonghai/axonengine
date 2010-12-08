/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

#include <mmsystem.h>
#include <Psapi.h>
#include "BugTrap.h"

AX_BEGIN_NAMESPACE

void CDECL Errorf(const char *fmt, ...)
{
	va_list argptr;
	int len;
	char buffer[4096];

	std::string err_msg = "Occurred critical error. There are error message:\n\n";

	va_start(argptr, fmt);
	len = StringUtil::vsnprintf(buffer, ArraySize(buffer), fmt, argptr);
	va_end(argptr);

	err_msg += buffer;
	std::wstring wstr = u2w(buffer);
	std::string lstr = w2l(wstr);
	wprintf(L"%s", wstr.c_str());
	OutputDebugStringW(wstr.c_str());

	fprintf(g_logFile, "\n====================================================\n");
	fprintf(g_logFile, "%s\n", err_msg.c_str());
	fprintf(g_logFile, "====================================================\n");

#if 0
	if (g_system) {
		g_system->print(S_COLOR_RED);
		g_system->print(err_msg.c_str());
	}
#endif
	// close log file
	if (g_logFile) {
		::fclose(g_logFile);
	}

	BTTrace trace(NULL, BTLF_TEXT);
	BT_AddLogFile(trace.GetFileName());
	trace.Clear();
	trace.Insert(BTLL_ERROR,  lstr.c_str());
	trace.Close();

#if 0
	MessageBoxW(NULL, wstr.c_str(), L"Error", MB_ICONERROR | MB_OK | MB_TASKMODAL);
	throw err_msg.c_str();
	throw err_msg.c_str();
#endif
	throw err_msg.c_str();

	*(int*)0 = 0;

	abort();
}

void CDECL Debugf(const char *fmt, ...)
{
	va_list argptr;
	int len;
	char buffer[1024];

	va_start(argptr, fmt);
	len = StringUtil::vsnprintf(buffer, ArraySize(buffer), fmt, argptr);
	va_end(argptr);

	std::wstring wstr = u2w(buffer);
	wprintf(L"%s", wstr.c_str());
	fprintf(g_logFile, "%s", buffer);

	if (g_system) {
		g_system->print(S_COLOR_YELLOW);
		g_system->print(buffer);
	}
}

void CDECL Printf(const char *fmt, ...)
{
	va_list argptr;
	int len;
	char buffer[1024];

	va_start(argptr, fmt);
	len = StringUtil::vsnprintf(buffer, ArraySize(buffer), fmt, argptr);
	va_end(argptr);

	printf("%s", buffer);
	fprintf(g_logFile, "%s", buffer);

	if (g_system) {
		g_system->print(buffer);
	}
}


#pragma comment(lib, "Winmm.lib")

#if 0
uint_t OsUtil::milliseconds()
{
#if 0
	return Microseconds()  / 1000;
#else
	static uint_t starttime = 0;
	if (!starttime) {
		timeBeginPeriod(1);
		starttime = timeGetTime();
		return 0;
	}

	return timeGetTime() - starttime;
#endif
}

ulonglong_t OsUtil::microseconds()
{
// on multiprocessor, this may be have bugs for QPC..... fix me
static LARGE_INTEGER starttime;
static LARGE_INTEGER freq;
const longlong_t microseconds_per_second = 1000000;

	if (!starttime.QuadPart) {
		QueryPerformanceFrequency(&freq);

		// if don't support high frequency timer, fire error
		if (!freq.QuadPart) {
			Errorf("Milliseconds: system doesn't support high-resolution performance counter\n");
		}

		QueryPerformanceCounter(&starttime);
		srand(starttime.LowPart);

		return 0;
	}

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return (now.QuadPart - starttime.QuadPart) * microseconds_per_second / freq.QuadPart;
}
#endif

AX_END_NAMESPACE
