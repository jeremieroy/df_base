#include <UnitTest++.h>
#include <ReportAssert.h>
#include <df/system/Timer.h>
#include <df/system/Thread.h>
#include <df/system/Mutex.h>
#include <df/system/Logger.h>

namespace {

TEST( test_Logger)
{	
	df::Logger logger;
	//logger.setOutputToFile(false);
	logger.setOutputToStdOut(false);
    bool initOK = logger.init();
    CHECK(initOK);
	
	//make sure string termination are well managed
	logger.log(df::Logger::LOG_INFO, "aaaaaaaaaaaaaaaaaaaaaa");
	logger.log(df::Logger::LOG_INFO, "bbb");
	logger.log(df::Logger::LOG_INFO, "cccccccccccccc");

	logger.log(df::Logger::LOG_INFO, "This is an INFO message");
	logger.log(df::Logger::LOG_DEBUG, "This is a DEBUG message");
	logger.log(df::Logger::LOG_ERROR, "This is an ERROR");
		
	logger.log(df::Logger::LOG_INFO, "This is a formatted message int:%i float:%f", 42, 3.1415);

	logger.log(df::Logger::LOG_INFO, "This is a multiline message\nThis is the second line\nThis is the third line" );
	logger.log(df::Logger::LOG_INFO, "This is a multiline formatted message\nThis is the second line int:%i float:%f\nThis is the third line", 42, 3.1415 );
		
	logger.logWithPrefix(df::Logger::LOG_INFO, "This is a prefix", "This is a message");
	logger.logWithPrefix(df::Logger::LOG_INFO, "This is a prefix", "This is a formatted message int:%i float:%f", 42, 3.1415);
	logger.logWithPrefix(df::Logger::LOG_INFO, "This is a prefix", "This is a multiline message\nThis is the second line\nThis is the third line" );
	logger.logWithPrefix(df::Logger::LOG_INFO, "This is a prefix", "This is a multiline formatted message\nThis is the second line int:%i float:%f\nThis is the third line", 42, 3.1415 );		
		
	//check what happen when we make a very long write
	// This should not crash and truncate nicely
 	std::string big_text;
 	big_text.resize(5000,'a');
 	logger.log(df::Logger::LOG_INFO, big_text.c_str());
	
	//uncommenting any of these should result in an assert since we cannot change the config after initialization
	//logger.setOutputToFile(false);
	//logger.setOutputToStdOut(false);	
	//logger.setMinLogLevel(df::Logger::LOG_INFO);
	//logger.setFolderPath("../MyLovelyLogs");
	//logger.setFilePrefix("LovelyPrefixs");

    logger.close();
}

const int NUM_THREAD = 4;
const int NUM_INCREMENT = 100;

static void test_logger_run(void* data)
{
	df::LoggerProxy* proxy = (df::LoggerProxy*)(data);
	for (int i = 0; i<NUM_INCREMENT; ++i)
	{	
		proxy->log((df::Logger::LogLevel) (rand()%3), "%i -- %s", i, "This is a test message that contains my thread name, my iteration number and me.");			
	}
}

/* Test the logger under heavy multithread solution
 * 4 threads logs continuously and concurrently --> the result should not be scrambled
*/
TEST( test_Logger_multithread)
{
	df::Logger logger;
    //uncomment this to see the result in console
    logger.setOutputToStdOut(false);
	logger.init();   

	df::LoggerProxy proxy(&logger, "Proxy");
    df::Thread* threads[NUM_THREAD];
      //create threads
    for(int i=0; i<NUM_THREAD; ++i)
    {
        threads[i] = new df::Thread(&test_logger_run, &proxy);
    }
    
    //wait for termination
    for(int i=0; i<NUM_THREAD; ++i)
    {
        threads[i]->join();
    }   
    
    //clean up
    for(int i=0; i<NUM_THREAD; ++i)
    {
        delete threads[i];
    }
}

}
