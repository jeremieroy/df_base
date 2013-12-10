#pragma once
#include <df/system/Export.h>

namespace df
{
/// Represents a high precision Time/ TimeSpan, can represent a negative timespan.
class DF_SYSTEM_API Time
{
public:
	Time():_microseconds(0){}

	float asSeconds() const { return _microseconds / 1000000.f; }
	float asMilliseconds() const { return _microseconds / 1000.f; }
	int64 asMicroseconds() const { return _microseconds; }

private:
	explicit Time(int64 microseconds): _microseconds(microseconds) {}
    friend DF_SYSTEM_API Time seconds(float);
    friend DF_SYSTEM_API Time milliseconds(float);
    friend DF_SYSTEM_API Time microseconds(int64);
    int64 _microseconds;
};

///return a Time from a number of seconds
inline DF_SYSTEM_API Time seconds(float seconds) { return Time(static_cast<int64>(seconds * 1000000)); }
///return a Time from a number of milliseconds
inline DF_SYSTEM_API Time milliseconds(float milliseconds) { return Time(static_cast<int64>(milliseconds) * 1000); }
///return a Time from a number of microseconds
inline DF_SYSTEM_API Time microseconds(int64 microseconds) { return Time(microseconds); }

inline DF_SYSTEM_API bool operator ==(Time left, Time right) { return left.asMicroseconds() == right.asMicroseconds(); }
inline DF_SYSTEM_API bool operator !=(Time left, Time right) { return left.asMicroseconds() != right.asMicroseconds(); }
inline DF_SYSTEM_API bool operator <(Time left, Time right) { return left.asMicroseconds() < right.asMicroseconds(); }
inline DF_SYSTEM_API bool operator >(Time left, Time right) { return left.asMicroseconds() > right.asMicroseconds(); }
inline DF_SYSTEM_API bool operator <=(Time left, Time right) { return left.asMicroseconds() <= right.asMicroseconds(); }
inline DF_SYSTEM_API bool operator >=(Time left, Time right) { return left.asMicroseconds() >= right.asMicroseconds(); }
inline DF_SYSTEM_API Time operator -(Time right) { return microseconds(-right.asMicroseconds()); }
inline DF_SYSTEM_API Time operator +(Time left, Time right) { return microseconds(left.asMicroseconds() + right.asMicroseconds()); }
inline DF_SYSTEM_API Time& operator +=(Time& left, Time right) { return left = left + right; }
inline DF_SYSTEM_API Time operator -(Time left, Time right) { return microseconds(left.asMicroseconds() - right.asMicroseconds()); }
inline DF_SYSTEM_API Time& operator -=(Time& left, Time right) { return left = left - right; }
inline DF_SYSTEM_API Time operator *(Time left, float right) { return seconds(left.asSeconds() * right); }
inline DF_SYSTEM_API Time operator *(Time left, int64 right) { return microseconds(left.asMicroseconds() * right); }
inline DF_SYSTEM_API Time operator *(float left, Time right) { return right * left; }
inline DF_SYSTEM_API Time operator *(int64 left, Time right) { return right * left; }
inline DF_SYSTEM_API Time& operator *=(Time& left, float right) { return left = left * right; }
inline DF_SYSTEM_API Time& operator *=(Time& left, int64 right) { return left = left * right; }
inline DF_SYSTEM_API Time operator /(Time left, float right) { return seconds(left.asSeconds() / right); }
inline DF_SYSTEM_API Time operator /(Time left, int64 right) { return microseconds(left.asMicroseconds() / right); }
inline DF_SYSTEM_API Time& operator /=(Time& left, float right) { return left = left / right; }
inline DF_SYSTEM_API Time& operator /=(Time& left, int64 right) { return left = left / right; }

} // namespace df
