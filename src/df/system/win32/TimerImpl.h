#pragma once
#include <df/platform.h>
#include <df/system/Time.h>
#include <windows.h>

// Uncomment this if you want to make your profiler more safe see below.
// May incurs a perf penalty ...
//#define FORCE_FIRST_CORE

namespace df
{
namespace priv
{

/// Windows Timer implementation
class TimerImpl
{
public :  
	static LARGE_INTEGER getFrequency()
	{
		LARGE_INTEGER frequency;
	    QueryPerformanceFrequency(&frequency);
        return frequency;
	}

    static Time getCurrentTime()
	{
		// Force the following code to run on first core
		// (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx)
	#ifdef FORCE_FIRST_CORE
		HANDLE currentThread = GetCurrentThread();
		DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);
	#endif

		// Get the frequency of the performance counter
		// (it is constant across the program lifetime)
		static LARGE_INTEGER frequency = getFrequency();

		// Get the current time
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

	#ifdef FORCE_FIRST_CORE
		// Restore the thread affinity
		SetThreadAffinityMask(currentThread, previousMask);
	#endif

		// Return the current time as microseconds
		return df::microseconds(1000000 * time.QuadPart / frequency.QuadPart);
	}

};

} // namespace priv
} // namespace df

