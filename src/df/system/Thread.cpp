#include <df/system/Thread.h>
#if defined(DF_PLATFORM_WIN)
    #include <df/system/win32/ThreadImpl.h>
#else
    #include <df/system/posix/ThreadImpl.h>
#endif

#include <cassert>

namespace df
{
namespace priv
{
void runnableEntryPoint(void* runnable)
{
	((Runnable*)runnable)->run();
}
}

Thread::Thread(Runnable* runnable)
{
    assert(runnable!=NULL && "A Runnable object cannot be NULL");	
	_threadImpl = new priv::ThreadImpl(&priv::runnableEntryPoint, runnable );
}

Thread::Thread(void (*functionPtr)(void *), void * userData)
{
	assert(functionPtr!=NULL && "A thread function cannot be NULL");
	_threadImpl = new priv::ThreadImpl(functionPtr, userData );
}

Thread::~Thread()
{
	join();    
}

void Thread::join()
{
	if(_threadImpl)
	{
		_threadImpl->join();
		delete _threadImpl;
		_threadImpl = NULL;
	}
}
 
uint32 Thread::getID()
{
	if(_threadImpl)
	{
		return _threadImpl->getID();
	}	
	return 0;
}

void Thread::terminate()
{
	if(_threadImpl)
	{
		_threadImpl->terminate();
	}	
}

/* Must be provided in the implementation
namespace this_thread 
{
void Thread::sleep(Time time);
void Thread::yield();
uint32 Thread::getID();
}
*/

} // namespace df