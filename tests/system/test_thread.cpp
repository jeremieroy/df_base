#include <UnitTest++.h>
#include <ReportAssert.h>
#include <df/system/Thread.h>
#include <df/system/Mutex.h>

namespace {

const int NUM_THREAD = 10;
const int NUM_INCREMENT = 10000;
struct SafeCounter
{
    df::Mutex mutex;
    int count;
    void increment()
    { 
        df::ScopedLock lock(mutex);
        count++;
    }
};

void threadFunc(void* userData)
{
    SafeCounter* counter = (SafeCounter*) userData;
    for(int i=0; i< NUM_INCREMENT; ++i) 
    {
        counter->increment();
        if(i%2==1)
        {
           df::this_thread::yield();
        }
        else
        {
            df::this_thread::sleep(df::microseconds(33));
        }
    }
}

TEST(test_Thread)
{
    SafeCounter counter;
    counter.count = 0;
    
    df::Thread* threads[NUM_THREAD];
    //create threads
    for(int i=0; i<NUM_THREAD; ++i)
    {
        threads[i] = new df::Thread(&threadFunc, &counter);
    }
    
    //wait for termination
    for(int i=0; i<NUM_THREAD; ++i)
    {
        threads[i]->join();
    }   
    
    CHECK(counter.count == NUM_THREAD * NUM_INCREMENT);

    //clean up
    for(int i=0; i<NUM_THREAD; ++i)
    {
        delete threads[i];
    }
}

}