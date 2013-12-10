#pragma once

#include <df/system/NonCopyable.h>
#include <windows.h>

namespace df
{
namespace priv
{

/// \brief Windows implementation of mutexes
class MutexImpl : NonCopyable
{
public :
	MutexImpl() { InitializeCriticalSection(&_mutex); }
	~MutexImpl() { DeleteCriticalSection(&_mutex); } 	
	void lock() { EnterCriticalSection(&_mutex); }
	void unlock() { LeaveCriticalSection(&_mutex); }
	bool tryLock() { return (TryEnterCriticalSection(&_mutex)? true : false); }

private :
    CRITICAL_SECTION _mutex; ///< Win32 handle of the mutex
};

} // namespace priv

} // namespace df
