#include <df/system/Logger.h>
#include <df/system/Mutex.h>
#include <df/system/Thread.h>
#include <cstring>
#include <cassert>
#include <ctime>
#include <cstdio>
#include <cstdarg>

#ifdef DF_PLATFORM_WIN
#include <direct.h>
#define snprintf _snprintf
#endif

//TODO use a generic thread local storage implementation instead of this
// Or at least a smarter lock strategy than the global lock :(
#if !defined(_TTHREAD_CPP0X_) && !defined(thread_local)
	#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
		#define thread_local __thread
		#define LOGGER_THREAD_LOCAL
	#else
		#define thread_local __declspec(thread)
		#define LOGGER_THREAD_LOCAL
	#endif
#endif

namespace df {

const size_t MAX_LOG_BUFFER_SIZE = 4096;

#ifdef LOGGER_THREAD_LOCAL
	thread_local static char g_log_buffer[MAX_LOG_BUFFER_SIZE];
#else 
	static char g_log_buffer[MAX_LOG_BUFFER_SIZE];
#endif


class Logger::PrivateData
{	
public:
	PrivateData():
	  outputToFile(true),
      outputToStdOut(true),	 
	  minLogLevel(Logger::LOG_DEBUG),
	  isInitialized(false),
	  pFile(0)
	  {
		  std::strcpy(folderPath,"_logs");
		  std::strcpy(filePrefix,"");
	  }

	bool outputToFile;
	bool outputToStdOut;
	Logger::LogLevel minLogLevel;
	bool isInitialized;
	char folderPath[256];
	char filePrefix[64];	
	FILE * pFile;

#ifndef LOGGER_THREAD_LOCAL
	Mutex bufferMutex;
#endif
		
	//trick to avoid implementing two log functions with variadic arguments and a single arg as difference
	void logWithPrefix(Logger::LogLevel level, const char* prefix,  const char* format, va_list args);
};

Logger::Logger()
{
	_data = new PrivateData();	
}

Logger::~Logger()
{
	close();
    delete _data;
}

void Logger::setOutputToFile(bool state)
{
	assert(!_data->isInitialized && "Config is immutable after init");	
	_data->outputToFile = state;
}
	
void Logger::setOutputToStdOut(bool state)
{
	assert(!_data->isInitialized && "Config is immutable after init");
	_data->outputToStdOut = state;
}
	
void Logger::setFolderPath(const char* folderPath)
{
	assert(!_data->isInitialized && "Config is immutable after init");	
	std::strcpy(_data->folderPath, folderPath);
}

void Logger::setFilePrefix(const char* prefix)
{
	assert(!_data->isInitialized && "Config is immutable after init");	
	std::strcpy(_data->filePrefix, prefix);
}

void Logger::setMinLogLevel(LogLevel level)
{
	assert(!_data->isInitialized && "Config is immutable after init");	
	_data->minLogLevel = level;
}

bool Logger::init()
{
#ifndef LOGGER_THREAD_LOCAL
	ScopedLock(_data->bufferMutex);
#endif
	char* bufferPtr = g_log_buffer;

	memset(bufferPtr,'*',MAX_LOG_BUFFER_SIZE);	
	bufferPtr[MAX_LOG_BUFFER_SIZE-1] = '\0';	

	//close file if opened
	if(_data->pFile != 0)
	{
		fclose(_data->pFile);
		_data->pFile = 0;
	}

	if(_data->outputToFile)
	{
		//create log folder if not created
		#if defined(_WIN32)
			_mkdir(_data->folderPath);
		#else 
			#pragma message("Check if this is good permissions")
			mkdir(_data->folderPath, 0755); 
		#endif

		//construct log file name
		strcpy(bufferPtr, _data->folderPath);
		strcat(bufferPtr, "/");
		strcat(bufferPtr, _data->filePrefix);
		strcat(bufferPtr, "_log.txt");		
		
		_data->pFile = fopen ( bufferPtr,"a");
		if(_data->pFile == 0) 
		{
			return false;
		}
		setbuf ( _data->pFile , NULL ); //force unbuffered

		//check whether the log file needs an initial header of not
		fseek(_data->pFile, 0, SEEK_END);
		if(ftell(_data->pFile) == 0)
		{
			const char header1[] = "HH:MM:SS thread Lvl : message\n";
			const char header2[] = "-----------------------------\n";
			fwrite(header1,1,sizeof(header1)-1, _data->pFile);
			fwrite(header2,1,sizeof(header2)-1, _data->pFile);
		}

		//Write session header
		std::time_t t = std::time(NULL);
		time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );

		size_t written = strftime ( bufferPtr, MAX_LOG_BUFFER_SIZE, "----- Start: %c -----\n", timeinfo);
		fwrite(bufferPtr,1,written, _data->pFile);
	}
	
	_data->isInitialized = true;
	
	return true;
}
void Logger::close()
{
    if(_data->pFile !=0) 
	{
		fclose (_data->pFile);
		_data->pFile = 0;
        _data->isInitialized = false;
	}    
}


void Logger::log(LogLevel level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	_data->logWithPrefix(level, NULL, format, args);
	va_end(args);	
}

void Logger::logWithPrefix(LogLevel level, const char* prefix,  const char* format, ...)
{
	va_list args;
	va_start(args, format);
	_data->logWithPrefix(level, prefix, format, args);
	va_end(args);
}

void Logger::PrivateData::logWithPrefix(Logger::LogLevel level, const char* prefix,  const char* format, va_list args)
{
#ifndef LOGGER_THREAD_LOCAL
	ScopedLock bufferLock(bufferMutex);
#endif

	// lock-free logging implementation (except locking inside std::ofstream)
	if (level > minLogLevel || !isInitialized)
		return;

	// *** allocate thread-local buffer here if we use a templated implementation***
	// ...
	char* start_buf = g_log_buffer;
	char* cur_buf = start_buf;
	
	int remainingSize = MAX_LOG_BUFFER_SIZE-2; // two bytes are kept for trailing \n\0
	
	//retrieve current time
	//TODO time() is not thread safe! but I don't see any bad side effect so far...
	std::time_t t = std::time(NULL);
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	static char const* const category_str[] = {"ERR", "INF", "DBG"};
	
	//get an id for the calling thread
	uint32 threadID = this_thread::getID();
	
	int lineHeaderSize;
	if(prefix != NULL){
		lineHeaderSize = snprintf(cur_buf, remainingSize, "%02i:%02i:%02i %6i %s [%s] ", 
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, threadID, category_str[level], prefix);
	}else
	{
		lineHeaderSize = snprintf(cur_buf, remainingSize, "%02i:%02i:%02i %6i %s ", 
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, threadID, category_str[level]);		
	}	
	cur_buf += lineHeaderSize;
	remainingSize -= lineHeaderSize;

	//append user message	
	int vsnWritten = vsnprintf(cur_buf, remainingSize, format, args);
	if (vsnWritten != -1)
	{
		cur_buf += vsnWritten;
		remainingSize -= vsnWritten;
	}else
	{
		cur_buf = start_buf+MAX_LOG_BUFFER_SIZE-2;
		remainingSize = 0;
	}
	
	
	//Align new line on  the msg header
	if(remainingSize > lineHeaderSize )
	{
		int endlineCount = 0;
		const char* scan_buf = format;
		while(*scan_buf!='\0')
		{
			endlineCount +=(*scan_buf=='\n');
			++scan_buf;
		}

		//is there enough space to add alignment space
		if(endlineCount > 0 && (remainingSize >  endlineCount * lineHeaderSize ))
		{
			//scan backward
			char* scan_buf = cur_buf-1;		
			char* prev_buf_end = scan_buf;

			//update state preemptively 
			remainingSize -= lineHeaderSize;
			cur_buf +=endlineCount * lineHeaderSize;

			while(scan_buf >= start_buf)
			{
				if(*scan_buf=='\n')
				{
					int count = prev_buf_end -scan_buf;
					char* tgt_buf = scan_buf+1+endlineCount*lineHeaderSize;
					// move text on the right to its final position
					memmove (tgt_buf, scan_buf+1, prev_buf_end -scan_buf);					
					// insert whitespace
					memset(tgt_buf - lineHeaderSize,' ',lineHeaderSize);
					--endlineCount;
					prev_buf_end = scan_buf;					
				}			
				--scan_buf;
			}
		}			
	}
	

	*cur_buf = '\n';
	++cur_buf;
	*cur_buf = '\0';

	//no need to check boolean, if file is open we can write	
	if(pFile != 0)
	{
		assert(outputToFile);
		fwrite(start_buf,1, cur_buf-start_buf, pFile);
	}

	if(outputToStdOut)
	{
		printf(start_buf);
	}
}

//****************************************************

void LoggerProxy::log(Logger::LogLevel level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	_logger->_data->logWithPrefix(level, _prefix, format, args);
	va_end(args);
}

void LoggerProxy::logError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	_logger->_data->logWithPrefix(Logger::LOG_ERROR, _prefix, format, args);
	va_end(args);
}

#ifdef _DEBUG
void LoggerProxy::logInfo(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	_logger->_data->logWithPrefix(Logger::LOG_INFO, _prefix, format, args);
	va_end(args);
}

void LoggerProxy::logDebug(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	_logger->_data->logWithPrefix(Logger::LOG_DEBUG, _prefix, format, args);
	va_end(args);
}
#endif

}
