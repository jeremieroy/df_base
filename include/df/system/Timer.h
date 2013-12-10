#pragma once
#include <df/system/Export.h>
#include <df/system/Time.h>

namespace df
{
/// Utility class that measures the elapsed time
class DF_SYSTEM_API Timer
{
public :
    /// The Timer starts automatically after being constructed.
    Timer();

	/// Return the elapsed time since the last restart (or constructor).
    Time getElapsedTime() const;

    /// Restart the timer
    Time restart();
private :
    Time _startTime;
};

} // namespace df