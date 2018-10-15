#ifndef TINYSERVER_LOGGER_H
#define TINYSERVER_LOGGER_H

#include <log4cxx/logger.h>
#include <cstdlib>		// abort()
#include <atomic>		// atomic_bool

namespace tinyserver
{
/**
 * 封装log4cxx的日志类
 * 可以直接使用下面的宏，将使用默认配置将日志打印在控制台
 * 也可以调用InitByFile()函数指定log4cxx的配置文件
 */
	class Logger
	{
	public:
		Logger();
		~Logger();

		static void InitByFile(const char *filePath);
		static log4cxx::LoggerPtr GetLogger();
	private:
		static log4cxx::LoggerPtr logger;
		static std::atomic_bool isInit;
	};
}

#define LOG_TRACE(message) do {											\
			LOG4CXX_TRACE(tinyserver::GetLogger(), message);			\
		} while(0);

#define LOG_DEBUG(message) do {											\
			LOG4CXX_DEBUG(tinyserver::Logger::GetLogger(), message);			\
		} while(0);

#define LOG_INFO(message) do {											\
			LOG4CXX_INFO(tinyserver::Logger::GetLogger(), message);			\
		} while(0);

#define LOG_WARN(message) do {											\
			LOG4CXX_WARN(tinyserver::Logger::GetLogger(), message);			\
		} while(0);

#define LOG_ERROR(message) do {											\
			LOG4CXX_ERROR(tinyserver::Logger::GetLogger(), message);			\
		} while(0);

#define LOG_FATAL(message) do {											\
			LOG4CXX_FATAL(tinyserver::Logger::GetLogger(), message);			\
			std::abort();												\
		} while(0);
#endif	// TINYSERVER_LOGGER_H
