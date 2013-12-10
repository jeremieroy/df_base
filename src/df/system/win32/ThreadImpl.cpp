#include <df/system/win32/ThreadImpl.h>
#include <df/system/Time.h>
#include <df/system/Thread.h>
#include <cassert>
#include <process.h>

namespace df
{
namespace priv
{

ThreadImpl::ThreadImpl(void (*functionPtr)(void *), void * userData):_threadId(0)
{
	_info.functionPtr = functionPtr;
	_info.userData = userData;

	_thread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, entryPoint, &_info, 0, &_threadId));
	assert(_thread != NULL && "Failed to create thread");
}

ThreadImpl::~ThreadImpl()
{
	if (_thread)
		CloseHandle(_thread);
}

void ThreadImpl::join()
{
	if (_thread)
	{
		assert(_threadId != GetCurrentThreadId() && "A thread cannot join itself");
		WaitForSingleObject(_thread, INFINITE);		
	}
}

uint32 ThreadImpl::getID()
{
	return _threadId;
}

void ThreadImpl::terminate()
{
	if (_thread)
		TerminateThread(_thread, 0);
}

unsigned int __stdcall ThreadImpl::entryPoint(void* userData)
{
	ThreadStartInfo* info = (ThreadStartInfo*) userData;
	info->functionPtr(info->userData);
	return 0;
}

} // namespace priv

namespace this_thread 
{
	void sleep(Time time) {  ::Sleep( (DWORD)(time.asMicroseconds()/1000)); }
	void yield(){  ::Sleep(0); }
	uint32 getID() { return (uint32) GetCurrentThreadId(); }
}
} // namespace df

