
#include "pch.h"
#include "private.h"

namespace AX {

/*
 *
 *  C++ Portable Types Library (PTypes)
 *  Version 2.0.2  Released 17-May-2004
 *
 *  Copyright (C) 2001-2004 Hovik Melikyan
 *
 *  http://www.melikyan.com/ptypes/
 *
 */

int FASTCALL Increment( int* target ) {
	int temp = 1;
	__asm__ __volatile ("lock ; xaddl %0,(%1)" : "+r" (temp) : "r" (target));
	return temp + 1;
}

int FASTCALL Decrement( int* target ) {
	int temp = -1;
	__asm__ __volatile ("lock ; xaddl %0,(%1)" : "+r" (temp) : "r" (target));
	return temp - 1;
}

//------------------------------------------------------------------------------
// class SyncMutex
//------------------------------------------------------------------------------


SyncMutex::SyncMutex()
	: mMutex( NULL )
{
	int	rc;
	
	mMutex = ::malloc( sizeof( pthread_mutex_t ) );
	
#if defined(__linux) && !defined(__USE_UNIX98)
    const pthread_mutexattr_t attr = { PTHREAD_MUTEX_RECURSIVE_NP };
#else
    pthread_mutexattr_t attr;
    rc = pthread_mutexattr_init( &attr );
	AX_ASSERT( rc == 0 );
	
	rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	AX_ASSERT( rc == 0 );
#endif
    
    rc = pthread_mutex_init( (pthread_mutex_t*)mMutex, &attr );
	AX_ASSERT( rc == 0 );

#if defined(__linux) && !defined(__USE_UNIX98)
// Nothing to do
#else
    rc = pthread_mutexattr_destroy( &attr );
	AX_ASSERT( rc == 0 );
#endif
}

SyncMutex::~SyncMutex() {
	int rc = ::pthread_mutex_destroy( (pthread_mutex_t*)mMutex );
	AX_ASSERT( rc == 0 );
	SafeFree( mMutex );
}

bool
SyncMutex::Lock( uint_t ) {
	int rc = ::pthread_mutex_lock( (pthread_mutex_t*)mMutex );
	if( rc != 0 ) {
		Errorf( _("SyncMutex::Lock: sys call error") );
	}
	return true;
}

bool
SyncMutex::Unlock() {
	pthread_mutex_unlock( (pthread_mutex_t*)mMutex );
	return true;
}

#if 0
Handle
SyncMutex::GetHandle() {
	return mMutex;
}
#endif

//------------------------------------------------------------------------------
// class SyncEvent
//------------------------------------------------------------------------------

struct EventData 
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
};

#define GETDATA	((EventData*)mEventHandle)

SyncEvent::SyncEvent() : mEventHandle( NULL ) {
	mEventHandle = malloc( sizeof( EventData ) );

	int				rc;
    pthread_condattr_t 	condattr;

	// init a mutex for this condition
#if defined(__linux) && !defined(__USE_UNIX98)
    const pthread_mutexattr_t mutexattr = { PTHREAD_MUTEX_TIMED_NP };
#else
    pthread_mutexattr_t attr;
    rc = pthread_mutexattr_init( &attr );
	AX_ASSERT( rc == 0 );
	
	rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	AX_ASSERT( rc == 0 );
#endif
    
    rc = pthread_mutex_init( &GETDATA->mutex, &attr );
	AX_ASSERT( rc == 0 );

#if defined(__linux) && !defined(__USE_UNIX98)
// Nothing to do
#else
    rc = pthread_mutexattr_destroy( &attr );
	AX_ASSERT( rc == 0 );
#endif

	// init pthread condition
    rc = pthread_condattr_init( &condattr );
	AX_ASSERT( rc == 0 );

    rc = pthread_cond_init( &GETDATA->cond, &condattr );
	AX_ASSERT( rc = 0 );
	
    rc = pthread_condattr_destroy( &condattr );
	AX_ASSERT( rc = 0 );
}

SyncEvent::~SyncEvent() {
	::pthread_mutex_destroy( &GETDATA->mutex );
	::pthread_cond_destroy( &GETDATA->cond );
	free( mEventHandle );
}

bool
SyncEvent::Lock( uint_t timeout ) {
	int rc;
	
	rc = ::pthread_mutex_lock( &GETDATA->mutex );
	AX_ASSERT( rc == 0 );
	rc = ::pthread_cond_wait( &GETDATA->cond, &GETDATA->mutex );
	AX_ASSERT( rc == 0 );
	rc = ::pthread_mutex_unlock( &GETDATA->mutex );
	AX_ASSERT( rc == 0 );
	
	return true;
}

bool
SyncEvent::Unlock() {
	return true;
}

bool
SyncEvent::SetEvent() {
	int rc;
	
	rc = ::pthread_mutex_lock( &GETDATA->mutex );
	AX_ASSERT( rc == 0 );
	rc = ::pthread_cond_signal( &GETDATA->cond );
	AX_ASSERT( rc == 0 );
	rc = ::pthread_mutex_unlock( &GETDATA->mutex );
	AX_ASSERT( rc == 0 );
	
	return true;
}
bool
SyncEvent::PulseEvent() {
	// can't do this on linux
	return true;
}
bool
SyncEvent::ResetEvent() {
	// can't do this on linux
	return true;
}

//------------------------------------------------------------------------------
// class Thread
//------------------------------------------------------------------------------
static void* ThreadProc( void* lpParameter ) {
	Thread* thread = (Thread*)lpParameter;
	thread->doRun();

	return 0;
}

Thread::Thread() {
	exitEvent_ = CxNew SyncEvent();

	handle_ = malloc( sizeof( pthread_t ) );
	
	pthread_attr_t threadAttr;
//	struct sched_param param;  // scheduling priority
	
	// initialize the thread attribute
	pthread_attr_init( &threadAttr );
	
	// Set the stack size of the thread
	pthread_attr_setstacksize( &threadAttr, 120*1024 );
	
	// Set thread to detached state. No need for pthread_join
	pthread_attr_setdetachstate( &threadAttr, PTHREAD_CREATE_DETACHED );
	
	// Create the threads
	pthread_create( (pthread_t*)handle_, &threadAttr, ThreadProc, NULL);
	
	// Destroy the thread attributes
	pthread_attr_destroy(&threadAttr);
}

void
Thread::startThread() {
}

void
Thread::endThread() {
	exitEvent_->SetEvent();
}


Thread::~Thread() {
	CxDelete( exitEvent_ );
	free( handle_ );
}


} // namespace AX

