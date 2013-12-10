#pragma once
#include <df/platform.h>
#include <df/system/NonCopyable.h>
#include <windows.h>

namespace df
{
namespace priv
{

/// windows thread implementation
class ThreadImpl : NonCopyable
{
public:
    ThreadImpl( void (*functionPtr)(void *), void * userData );
    ~ThreadImpl();

	void join();
    uint32 getID();
	void terminate();
private:
	HANDLE _thread; ///< Win32 thread handle
	uint32 _threadId; ///< Win32 thread identifier

	struct ThreadStartInfo {
		void (*functionPtr)(void *); ///< Pointer to the function to be executed.
		void * userData;            ///< Function argument for the thread function.
	} _info;
	static unsigned int __stdcall entryPoint(void* userData);
};

} // namespace priv
} // namespace df

