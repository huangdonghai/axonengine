/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

#include <windows.h>

AX_BEGIN_NAMESPACE

namespace {

	typedef Dict<ulong_t, Thread*> ThreadDict;
	static ThreadDict m_threadDict;

	// TODO: not multi thread safe
	static DWORD GetTlsId()
	{
		static DWORD tlsId = 0;

		if (tlsId == 0) {
			tlsId = ::TlsAlloc();
		}

		return tlsId;
	}

	static void SetCurrentThread(Thread *thread)
	{
		DWORD tlsid = GetTlsId();
		::TlsSetValue(tlsid, thread);
	}

	static Thread *GetCurrentThread()
	{
		DWORD tlsid = GetTlsId();
		Thread *thread = (Thread*)::TlsGetValue(tlsid);

		if (thread)
			return thread;

		DWORD threadId = ::GetCurrentThreadId();
		thread = Thread::getThreadById(threadId);

		AX_ASSERT(thread);
		SetCurrentThread(thread);

		return thread;
	}
} // namespace

Thread *Thread::ms_mainThread = 0;
Stat stat_lockTimes("Client", "LockTimes", Stat::F_Int|Stat::F_AutoReset, "how many times synclock called");

//--------------------------------------------------------------------------
// class SyncMutex
//--------------------------------------------------------------------------

SyncMutex::SyncMutex()
	: m_object(NULL)
{
	m_object = ::malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)m_object);
}

SyncMutex::~SyncMutex() {
	DeleteCriticalSection((LPCRITICAL_SECTION)m_object);
	SafeFree(m_object);
}

bool SyncMutex::lock(uint_t) {
	EnterCriticalSection((LPCRITICAL_SECTION)m_object);
	stat_lockTimes.inc();
	return true;
}

bool SyncMutex::unlock() {
	LeaveCriticalSection((LPCRITICAL_SECTION)m_object);
	return true;
}

#if 0
Handle
SyncMutex::GetHandle() {
	return mMutex;
}
#endif
//--------------------------------------------------------------------------
// class SyncEvent
//--------------------------------------------------------------------------

SyncEvent::SyncEvent() : m_object(NULL) {
	m_object = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	AX_ASSERT(m_object);
}

SyncEvent::~SyncEvent() {
	::CloseHandle(m_object);
}

bool SyncEvent::lock(uint_t timeout) {
	DWORD dwRet = ::WaitForSingleObject(m_object, timeout);
	if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
		return TRUE;
	else
		return FALSE;
}

bool SyncEvent::unlock() {
	return true;
}
#if 0
Handle
SyncEvent::GetHandle() {
	return mEventHandle;
}
#endif
bool SyncEvent::setEvent() {
	return ::SetEvent(m_object) ? true : false;
}
bool SyncEvent::pulseEvent() {
	return ::PulseEvent(m_object) ? true : false;
}
bool SyncEvent::resetEvent() {
	return ::ResetEvent(m_object) ? true : false;
}

//--------------------------------------------------------------------------
// class Thread
//--------------------------------------------------------------------------
//#include "BugTrap.h"

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
//	BT_SetTerminate(); // set_terminate() must be called from every thread
	Thread *thread = (Thread*)lpParameter;

	while (1) {
		thread->dispatchAsyncNotify();

		if (thread->doRun() == Thread::RS_Exit)
			break;
	}

	return 0;
}

Thread::Thread()
{
	checkMainThread();

	m_exitEvent = new SyncEvent();
	m_handle = Handle(::CreateThread(NULL, 0, ThreadProc, this, CREATE_SUSPENDED, &m_id));
	m_id = ::GetThreadId(m_handle.castTo<HANDLE>());
	AX_ASSERT(m_handle);

	m_threadDict[m_id] = this;
}

Thread::Thread(ulong_t id)
{
	m_id = id;
	m_handle = 0;
	m_exitEvent = 0;

	m_threadDict[m_id] = this;
}

Thread *Thread::getThreadById(ulong_t id)
{
	ThreadDict::const_iterator it = m_threadDict.find(id);

	if (it != m_threadDict.end()) {
		return it->second;
	} else {
		Thread *newthread = new Thread(id);
		return newthread;
	}
}


Thread *Thread::getCurrentThread()
{
	return GetCurrentThread();
}

void Thread::startThread()
{
	::ResumeThread(m_handle.castTo<HANDLE>());
}

void Thread::stopThread()
{
	m_exitEvent->setEvent();
}

Thread::~Thread()
{
	::CloseHandle(m_handle.castTo<HANDLE>());
	delete(m_exitEvent);
}

bool Thread::isCurrentThread() const
{
	return GetCurrentThreadId() == m_id;
}


void Thread::checkMainThread()
{
	Thread *t = getCurrentThread();

	if (!t->m_handle) {
		ms_mainThread = t;
	}
}



AX_END_NAMESPACE
