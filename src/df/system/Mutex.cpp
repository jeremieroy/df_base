#include <df/system/Mutex.h>

#if defined(DF_PLATFORM_WIN)
    #include <df/system/win32/MutexImpl.h>
#else
    #include <df/system/posix/MutexImpl.h>
#endif


namespace df
{
Mutex::Mutex()
{
    _mutexImpl = new priv::MutexImpl;
}

Mutex::~Mutex()
{
    delete _mutexImpl;
}

void Mutex::lock()
{
    _mutexImpl->lock();
}

void Mutex::unlock()
{
    _mutexImpl->unlock();
}

bool Mutex::tryLock()
{
    return _mutexImpl->tryLock();
}

} // namespace df
