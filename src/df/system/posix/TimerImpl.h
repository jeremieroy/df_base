#pragma once
#include <df/platform.h>
#include <df/system/Time.h>
#ifdef DF_PLATFORM_OSX
    #include <mach/mach_time.h>
#else
    #include <time.h>
#endif

namespace df
{
namespace priv
{

/// Posix Timer implementation
class TimerImpl
{
public :   
    static Time getCurrentTime()
	{
		#ifdef DF_PLATFORM_OSX
		// Mac OS X specific implementation (it doesn't support clock_gettime)
		static mach_timebase_info_data_t frequency = {0, 0};
		if (frequency.denom == 0)
			mach_timebase_info(&frequency);
		Uint64 nanoseconds = mach_absolute_time() * frequency.numer / frequency.denom;
		return df::microseconds(nanoseconds / 1000);

	#else
		// POSIX implementation
		timespec time;
		clock_gettime(CLOCK_MONOTONIC, &time);
		return df::microseconds(static_cast<uint64>(time.tv_sec) * 1000000 + time.tv_nsec / 1000);
	#endif
	}
};

} // namespace priv

} // namespace df
