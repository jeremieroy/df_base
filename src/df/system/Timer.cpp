#include <df/system/Timer.h>

#if defined(DF_PLATFORM_WIN)
    #include <df/system/win32/TimerImpl.h>
#else
    #include <df/system/posix/TimerImpl.h>
#endif

namespace df
{
Timer::Timer() :
_startTime(priv::TimerImpl::getCurrentTime())
{
}

Time Timer::getElapsedTime() const
{
    return priv::TimerImpl::getCurrentTime() - _startTime;
}

Time Timer::restart()
{
    Time now = priv::TimerImpl::getCurrentTime();
    Time elapsed = now - _startTime;
    _startTime = now;

    return elapsed;
}

} // namespace df
