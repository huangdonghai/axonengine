/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

AX_BEGIN_NAMESPACE

#ifdef _WIN32
#	include <windows.h>
#else
#	include <pthread.h>
#endif


void Thread::addAsyncNotify(INotifyHandler *handler, int index)
{
	ScopedLocker autoLocker(m_asyncNotifyMutex);

	AsyncNotify an;
	an.handler = handler;
	an.index = index;

	m_asyncNotifyList.push_back(an);
}

void Thread::dispatchAsyncNotify()
{
	ScopedLocker autoLocker(m_asyncNotifyMutex);

	std::list<AsyncNotify>::const_iterator it = m_asyncNotifyList.begin();

	for (; it != m_asyncNotifyList.end(); ++it) {
		it->handler->notify(it->index);
	}

	m_asyncNotifyList.clear();
}

AX_END_NAMESPACE
