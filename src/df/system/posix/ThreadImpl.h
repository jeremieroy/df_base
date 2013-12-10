#pragma once
#include <df/platform.h>
#include <df/system/NonCopyable.h>
#include <pthread.h>

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
	void terminate()
private:
	pthread_t _thread; ///< posix thread handle
	uint32 _threadId; ///<  thread identifier
	
	struct ThreadStartInfo {
		void (*functionPtr)(void *); ///< Pointer to the function to be executed.
		void * userData;            ///< Function argument for the thread function.
	} _info;
	static void* entryPoint(void* userData);	
};

static uint32 pthread_t_to_ID(const pthread_t &handle);

} // namespace priv
} // namespace df

