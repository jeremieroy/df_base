#include <UnitTest++.h>
#include <ReportAssert.h>
#include <df/system/Timer.h>
#include <df/system/Thread.h>

namespace {

TEST(test_Time)
{
    df::Time t_seconds = df::seconds(1.0f);
    df::Time t_milliseconds = df::milliseconds(1000.0f);
    df::Time t_microseconds = df::microseconds(1000000);
    CHECK_CLOSE( t_seconds.asMicroseconds(), t_milliseconds.asMicroseconds(), 10);
    CHECK_CLOSE( t_seconds.asMicroseconds(), t_microseconds.asMicroseconds(), 10);
    CHECK_CLOSE( t_milliseconds.asMicroseconds(), t_microseconds.asMicroseconds(), 10);
}

TEST(test_Timer)
{
    df::Timer timer;
    df::Time t = timer.getElapsedTime();
    df::this_thread::sleep(df::milliseconds(100.0f));
    df::Time t2 = timer.getElapsedTime();
    df::Time elapsed = t2 -t;
    // makee loose estimate because OS sleep is imprecise by design :(
    CHECK( elapsed.asMilliseconds() >= 75.0f);
    CHECK( elapsed.asMilliseconds() <= 125.0f);
}

}