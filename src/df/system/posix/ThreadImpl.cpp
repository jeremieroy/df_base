#include <df/system/win32/ThreadImpl.h>
#include <df/system/Time.h>
#include <df/system/Thread.h>
#include <cassert>
#include <process.h>
#include <map>

namespace df
{
namespace priv
{

ThreadImpl::ThreadImpl(void (*functionPtr)(void *), void * userData):_threadId(0)
{
	_info.functionPtr = functionPtr;
	_info.userData = userData;
	if(pthread_create(&_thread, NULL, &entryPoint, _info) != 0)
	{
		_thread = 0;
		assert(false && "Failed to create thread");
	}
}

ThreadImpl::~ThreadImpl()
{
	//nothing to clean up (join has been called)
}

void ThreadImpl::join()
{
	if (_thread )
	{	
		assert( (pthread_equal(pthread_self(), _thread) == 0) && "A thread cannot join itself");
		pthread_join(m_thread, NULL);
	}
}

uint32 ThreadImpl::getID()
{
	return _pthread_t_to_ID(mHandle);
	return _threadId;
}

void ThreadImpl::terminate()
{
	if (_thread)
		pthread_cancel(_thread);

}

void* ThreadImpl::entryPoint(void* userData)
{
	ThreadStartInfo* info = (ThreadStartInfo*) userData;
	// Tell the thread to handle cancel requests immediately
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	info->functionPtr(info->userData);
	return NULL;
}

uint32 pthread_t_to_ID(const pthread_t &handle)
{
	static Mutex idMapLock;
	static std::map<pthread_t, uint32> idMap;
	static uint32 idCount(1);

	ScopedLock guard(idMapLock);
	if(idMap.find(handle) == idMap.end())
		idMap[handle] = idCount ++;
	return idMap[handle];
}

} // namespace priv

namespace this_thread 
{
	void sleep(Time time)
	{
		struct timespec sleepTime;
		struct timespec time_left_to_sleep;
		int32 seconds = int32(time.asMicroseconds() / 1000000)
		sleepTime.tv_sec = seconds;
		sleepTime.tv_nsec = 1000*(time.asMicroseconds()-seconds*1000000);
		//sleepTime.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
		//sleepTime.tv_sec = tv.tv_sec + (usecs / 1000000) + (ti.tv_nsec / 1000000000);
		//sleepTime.tv_nsec %= 1000000000;

		while( (sleepTime.tv_sec + sleepTime.tv_nec) > 0 )
		{
			nanosleep(&sleepTime, &time_left_to_sleep);
			sleepTime.tv_sec = time_left_to_sleep.tv_sec;
			sleepTime.tv_msec = time_left_to_sleep.tv_nsec;
		}

		//ALT 1
		//usleep is deprecated and may sometimes block the whole process (according to some internet rumors)
		//uint64 usecs = time.asMicroseconds();
		//usleep(int(usecs));

		//ALT 2
		// use pthread_cond_timedwait instead
		// this implementation is inspired from Qt
		/*
		// get the current time
		timeval tv;
		gettimeofday(&tv, NULL);

		// construct the time limit (current time + time to wait)
		timespec ti;
		ti.tv_nsec = (tv.tv_usec + (usecs % 1000000)) * 1000;
		ti.tv_sec = tv.tv_sec + (usecs / 1000000) + (ti.tv_nsec / 1000000000);
		ti.tv_nsec %= 1000000000;

		// create a mutex and thread condition
		pthread_mutex_t mutex;
		pthread_mutex_init(&mutex, 0);
		pthread_cond_t condition;
		pthread_cond_init(&condition, 0);

		// wait...
		pthread_mutex_lock(&mutex);
		pthread_cond_timedwait(&condition, &mutex, &ti);
		pthread_mutex_unlock(&mutex);

		// destroy the mutex and condition
		pthread_cond_destroy(&condition);
		pthread_mutex_destroy(&mutex);
		*/
	}

	void yield(){  sched_yield(); }
	uint32 getID() { return priv::pthread_t_to_ID(pthread_self());}
}
} // namespace df

