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

	void CDECL Errorf(const char* fmt, ...)
	{
		va_list argptr;
		int len;
		char buffer[4096];

		String err_msg = "Occurred critical error. There are error message:\n\n";

		va_start(argptr, fmt);
		len = StringUtil::vsnprintf(buffer, ArraySize(buffer), fmt, argptr);
		va_end(argptr);

		err_msg += buffer;
		WString wstr = u2w(buffer);
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
		trace.Insert(BTLL_ERROR,  wstr.c_str());
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

	void CDECL Debugf(const char* fmt, ...)
	{
		va_list argptr;
		int len;
		char buffer[1024];

		va_start(argptr, fmt);
		len = StringUtil::vsnprintf(buffer, ArraySize(buffer), fmt, argptr);
		va_end(argptr);

		WString wstr = u2w(buffer);
		wprintf(L"%s", wstr.c_str());
		fprintf(g_logFile, "%s", buffer);

		if (g_system) {
			g_system->print(S_COLOR_YELLOW);
			g_system->print(buffer);
		}
	}

	void CDECL Printf(const char* fmt, ...)
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

	String OsUtil::getClipboardString()
	{
		String out;
		wchar_t* data;

		if (!OpenClipboard(NULL))
			return out;

		data = (wchar_t*)GetClipboardData(CF_UNICODETEXT);
		if (data && GlobalLock(data)) {
			out = w2u(data);
			GlobalUnlock(data);
		}

		CloseClipboard();

		return out;
	}

	handle_t OsUtil::loadSysDll(const String& name)
	{
		String libname = name + ".dll";

		handle_t hDll = (handle_t)::LoadLibraryW(u2w(libname).c_str());
		if (NULL == hDll) {
			Errorf("LoadSysDll: can't load dll [%s]\n", libname.c_str());
		}
		return hDll;
	}

	handle_t OsUtil::loadDll(const String& name)
	{
	#if 0
		String libname = name + "D";
		return LoadSysDll(libname);
	#else
		return loadSysDll(name);
	#endif
	}

	void* OsUtil::getProcEntry(handle_t handle, const String& name)
	{
		return (void *)::GetProcAddress((HMODULE)handle, name.c_str());
	}

	bool OsUtil::freeDll(handle_t handle)
	{
		return ::FreeLibrary((HMODULE)handle) ? true : false;
	}

	#include "Iphlpapi.h"
	#pragma comment(lib,"Iphlpapi.lib")

	static bool __inited;
	static bool __haveit;
	static byte_t __macAddress[6];

	bool OsUtil::getMacAddress(byte_t address[6])
	{
		if (__inited) {
			memcpy(address, __macAddress, 6);
			return __haveit;
		}

		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		DWORD dwRetVal = 0;
		DWORD dwMinIndex = 0xFFFFFFFF;

		pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));
		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

		// Make an initial call to GetAdaptersInfo to get
		// the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			free(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
			pAdapter = pAdapterInfo;
			while (pAdapter) {
				if (pAdapter->Index < dwMinIndex) {
					memcpy(__macAddress, &pAdapter->Address[0], 6);
					dwMinIndex = pAdapter->Index;
				}
				pAdapter = pAdapter->Next;
				__haveit = true;
			}
		} else {
			// or we return a rand address
			Debugf("getMacAddress: can't find network adapter\n");

			// trigger srand
			milliseconds();

			// rand
			int rand1 = rand();
			int rand2 = rand();
			int rand3 = rand();

			__macAddress[0] = rand1 & 0xFF;
			__macAddress[1] = (rand1 >> 8) & 0xFF;
			__macAddress[2] = rand2 & 0xFF;
			__macAddress[3] = (rand2 >> 8) & 0xFF;
			__macAddress[4] = rand3 & 0xFF;
			__macAddress[5] = (rand3 >> 8) & 0xFF;
			__haveit = false;
		}

		// add by mahdi
		SafeFree(pAdapterInfo);

		memcpy(address, __macAddress, 6);
		return __haveit;
	}

	String OsUtil::getHostName()
	{
		return "";
	}

	int OsUtil::getCpuUsage()
	{
		static DWORD64 dw64LastTime = 0;
		DWORD64 dw64CurrTime = 0;

		FILETIME ftCurrUserTime = { 0 };
		FILETIME ftCurrKernelTime = { 0 };
		FILETIME ftTemp = { 0 };

		DWORD64 dw64CurrCPUTime = 0;
		static DWORD64 dw64LastCPUTime = 0;

		if (GetProcessTimes(GetCurrentProcess(), &ftTemp, &ftTemp, &ftCurrKernelTime, &ftCurrUserTime)) {
			DWORD64 dw64Temp1, dw64Temp2 = 0;

			dw64Temp1 = ftCurrUserTime.dwHighDateTime;
			dw64Temp1 <<= 32;
			dw64Temp1 |= ftCurrUserTime.dwLowDateTime;

			dw64Temp2 = ftCurrKernelTime.dwHighDateTime;
			dw64Temp2 <<= 32;
			dw64Temp2 |= ftCurrKernelTime.dwLowDateTime;

			dw64CurrCPUTime = dw64Temp1 + dw64Temp2;

			GetSystemTimeAsFileTime(&ftTemp);

			dw64CurrTime = ftTemp.dwHighDateTime;
			dw64CurrTime <<= 32;
			dw64CurrTime |= ftTemp.dwLowDateTime;

			DWORD dwTemp1 = (DWORD)(dw64CurrCPUTime - dw64LastCPUTime);
			DWORD dwTemp2 = (DWORD)(std::max<DWORD>((dw64CurrTime - dw64LastTime), 1));

			int result = 100.0f * (float)dwTemp1 / (float)dwTemp2;

			dw64LastTime = dw64CurrTime;

			dw64LastCPUTime = dw64CurrCPUTime;

			return result;
		} else {
			return 0;
		}
	}

#pragma comment(lib, "Psapi.lib")

	int OsUtil::getMemoryUsage()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
	  
	   //获取这个进程的内存使用情况。
		if (::GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
#if 0
			const int nBufSize = 512;
			TCHAR chBuf[nBufSize];

			ZeroMemory(chBuf,nBufSize);

			wsprintf(chBuf,L"\t缺页中断次数: 0x%08X\n", pmc.PageFaultCount);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t使用内存高峰: 0x%08X\n", pmc.PeakWorkingSetSize);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t当前使用的内存: 0x%08X\n", pmc.WorkingSetSize);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t使用页面缓存池高峰: 0x%08X\n", pmc.QuotaPeakPagedPoolUsage);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t使用页面缓存池: 0x%08X\n",  pmc.QuotaPagedPoolUsage);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t使用非分页缓存池高峰: 0x%08X\n", pmc.QuotaPeakNonPagedPoolUsage);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t使用非分页缓存池: 0x%08X\n", pmc.QuotaNonPagedPoolUsage);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t使用分页文件: 0x%08X\n", pmc.PagefileUsage);
			OutputDebugString(chBuf);

			wsprintf(chBuf,L"\t使用分页文件的高峰: 0x%08X\n", pmc.PeakPagefileUsage);
			OutputDebugString(chBuf);
#endif
			return pmc.WorkingSetSize;
		}
		return 0;
	}

	void OsUtil::sleep(int ms)
	{
		::Sleep(ms);
	}

AX_END_NAMESPACE
