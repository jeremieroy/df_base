#pragma once
#include <df/system/Export.h>
#include <df/system/NonCopyable.h>

namespace df
{
namespace priv
{
    class MutexImpl;
}

/// \brief Recursive mutex
class DF_SYSTEM_API Mutex : NonCopyable
{
public :
    Mutex();
    ~Mutex();

    void lock();
    void unlock();
	bool tryLock();

private :    
    priv::MutexImpl* _mutexImpl; ///< OS-specific implementation
};

/// RAII scoped lock
class DF_SYSTEM_API ScopedLock : NonCopyable
{
public :
	explicit ScopedLock(Mutex& mutex):_mutex(mutex) { _mutex.lock(); }
    ~ScopedLock(){ _mutex.unlock(); }
private :
    Mutex& _mutex;
};

} // namespace df
