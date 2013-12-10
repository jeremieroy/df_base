#pragma once
#include <df/system/Export.h>
#include <df/system/NonCopyable.h>

namespace df {

/// Log message to a file and/or stdout
/// remark any configuration change must be made prior to explicit initialization 
class DF_SYSTEM_API Logger : public NonCopyable
{
public:
	//! severity level of a log message
	enum LogLevel{ LOG_ERROR, LOG_INFO, LOG_DEBUG};
	
	Logger();
	~Logger();

	//! set whether or not logs must be written to a file (default: true)
	void setOutputToFile(bool state);
	//! set whether or not logs must be written to stdout (default: true)
	void setOutputToStdOut(bool state);	
	//! set the minimum log level that must be accepted by this logger (default: LOG_DEBUG)
	void setMinLogLevel(LogLevel level);
	//! set the path of the folder where logs must be written (max 255 characters) (default: "_logs")
	void setFolderPath(const char* folderPath);
	//! set a prefix that must be applied to the log fil name for this logger (max  63 characters) (default: "")
	void setFilePrefix(const char* prefix);

	/*! initialize the logger
	 *  /return true if the initialization succeed, false otherwise (most pbly because it was unable to create the requested log file)
	 * /remark any configuration change must be made prior to explicit initialization 
	*/
	bool init();

    //! close the logger
    void close();
	
	//! log a message using printf format
	void log(LogLevel level, const char* format, ...);
	//! log a message using printf format, but precede the message with a prefix (e.g. function name, subsystem ...)
	void logWithPrefix(LogLevel level, const char* prefix,  const char* format, ...);	
private:
	class PrivateData;
	PrivateData* _data;
	friend class LoggerProxy;
};

/// A proxy to the logger that help to avoid log call during runtime.
/// Also scope logs in the logProxy Name
class DF_SYSTEM_API LoggerProxy
{
public:
	LoggerProxy():_logger(0), _prefix(0){}
	LoggerProxy(Logger* logger, const char* prefix):_logger(logger), _prefix(prefix){}

	void log(Logger::LogLevel level, const char* format, ...);

	void logError(const char* format, ...);
#ifdef _DEBUG
	void logInfo(const char* format, ...);
	void logDebug(const char* format, ...);
#else
	void logInfo(const char* format, ...){};
	void logDebug(const char* format, ...){};
#endif
	Logger* _logger;
	const char* _prefix;
};

}
