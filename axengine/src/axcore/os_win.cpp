/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "private.h"

#include <direct.h>
#include <io.h>
#include <windows.h>
#include <mmsystem.h>
#include <Psapi.h>

AX_BEGIN_NAMESPACE


/*-----------------------------------------------------------------------------
	Nonstandard C API wrapper
-----------------------------------------------------------------------------*/
bool OsUtil::mkdir(const char *dirname )
{
	WString wdirname = u2w(dirname );
	int ret = ::_wmkdir(wdirname.c_str() );

	if (ret == -1 ) {
		errno_t err;
		_get_errno(&err );

		if (err == EEXIST ) {
			//Debugf("mkdir: Directory was not created because '%s' is the name of an existing file, directory, or device.\n", dirname );
			return false;
		} else if (err == ENOENT ) {
			//Debugf("mkdir(%s): Path was not found.\n", dirname );
			mkdir(PathUtil::removeFilename(dirname ).c_str() );
		}

		return mkdir(dirname );
	}

	return true;
}

String OsUtil::getworkpath()
{
	wchar_t buf[260];
	wchar_t * v = ::_wgetcwd(buf, ArraySize(buf));
	AX_ASSURE(v);
	return w2u(buf);
}


int OsUtil::getScreenWidth()
{
	return ::GetSystemMetrics(SM_CXSCREEN);
}

int OsUtil::getScreenHeight()
{
	return ::GetSystemMetrics(SM_CYSCREEN);
}

static longlong_t s_starttime = 0;
static longlong_t s_freq = 0;
static double s_invFreq = 0;

inline void checkTimeStart()
{
	// on multiprocessor, this may be have bugs for QPC..... fix me
	LARGE_INTEGER starttime;
	LARGE_INTEGER freq;

	if (!s_starttime) {
		QueryPerformanceFrequency(&freq);

		// if don't support high frequency timer, fire error
		if (!freq.QuadPart) {
			Errorf("Milliseconds: system doesn't support high-resolution performance counter\n");
		}

		QueryPerformanceCounter(&starttime);
		srand(starttime.LowPart);

		s_starttime = starttime.QuadPart;
		s_freq = freq.QuadPart;
		s_invFreq = 1.0 / freq.QuadPart;
	}
}

double OsUtil::cycleSeconds()
{
	checkTimeStart();

	return s_invFreq;
}

longlong_t OsUtil::cycles()
{
	checkTimeStart();

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return (now.QuadPart - s_starttime);
}


double OsUtil::seconds()
{
	checkTimeStart();

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);

	return (now.QuadPart - s_starttime) * s_invFreq;
}


String OsUtil::getClipboardString()
{
	String out;
	wchar_t *data;

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

Handle OsUtil::loadSysDll(const String &name)
{
	String libname = name + ".dll";

	Handle hDll = (Handle)::LoadLibraryW(u2w(libname).c_str());
	if (!hDll) {
		Errorf("LoadSysDll: can't load dll [%s]\n", libname.c_str());
	}
	return hDll;
}

Handle OsUtil::loadDll(const String &name)
{
#if 0
	String libname = name + "D";
	return LoadSysDll(libname);
#else
	return loadSysDll(name);
#endif
}

void *OsUtil::getProcEntry(Handle handle, const String &name)
{
	return (void *)::GetProcAddress(handle.to<HMODULE>(), name.c_str());
}

bool OsUtil::freeDll(Handle handle)
{
	return ::FreeLibrary(handle.to<HMODULE>()) ? true : false;
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
		seconds();

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

void OsUtil::sleep(float seconds)
{
	::Sleep(seconds * 1000);
}



AX_END_NAMESPACE
