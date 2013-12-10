#pragma once
#include <df/system/Export.h>

namespace df
{
/// \brief Utility class that makes any derived class non-copyable
class DF_SYSTEM_API NonCopyable
{
protected :
    NonCopyable() {}
private :
    NonCopyable(const NonCopyable&);
    NonCopyable& operator =(const NonCopyable&);
};

} // namespace df
