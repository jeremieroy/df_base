#pragma once
#include <df/system/Export.h>
#include <df/system/NonCopyable.h>
#include <df/system/Time.h>

namespace df
{
namespace priv { class ThreadImpl; }

class DF_SYSTEM_API Runnable
{
public:
	virtual void run() = 0;
	virtual ~Runnable(){}
};

/// \brief Thread class: 
class DF_SYSTEM_API Thread : NonCopyable
{
public:
	/// create a thread that will execute the run function of the Runnable object
    Thread(Runnable* runnable);
	/// create a thread that will execute the function given as argument
	Thread(void (*functionPtr)(void *), void * userData);
    ~Thread();

	/// wait for a thread to terminate
    void join();

    /// return the thread ID of a thread object
    uint32 getID();	
	
	/// Force a thread to interrupt its work and exit.
	/// There are few good reason to use this as there are no way to know what was the state of the thread before exit.
	/// Prefer to return from the thread call.
	void terminate();
private:	
    priv::ThreadImpl* _threadImpl; ///< OS-specific implementation	
};

namespace this_thread 
{
///Make the current thread sleep for a given duration
DF_SYSTEM_API void sleep(Time time);

/// Yield execution to another thread.
/// Offers the operating system the opportunity to schedule another thread
/// that is ready to run on the current processor.
DF_SYSTEM_API void yield();

/// Return the thread ID of the calling thread.
DF_SYSTEM_API uint32 getID();
}

} // namespace df