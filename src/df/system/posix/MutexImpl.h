#pragma once

#include <df/system/NonCopyable.h>
#include <pthread.h>

namespace df
{
namespace priv
{

/// \brief Unix implementation of mutexes
class MutexImpl : NonCopyable
{
public :
    MutexImpl()
	{		
		// Make it recursive to follow the windows behaviour
		pthread_mutexattr_t attributes;
		pthread_mutexattr_init(&attributes);
		pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&_mutex, &attributes);
	}
	~MutexImpl() {  pthread_mutex_destroy(&_mutex); }

	void lock() { pthread_mutex_lock(&_mutex); }
	void unlock() {  pthread_mutex_unlock(&_mutex); }
	bool trylock() {  return (pthread_mutex_trylock(&_mutex) == 0) ? true : false; }

private :
    pthread_mutex_t _mutex; ///< pthread handle of the mutex
};

} // namespace priv

} // namespace df
