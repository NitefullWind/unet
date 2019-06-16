#ifndef TINYSERVER_LOGGER_H
#define TINYSERVER_LOGGER_H

#include <atomic>		// atomic_bool
#include <cstdlib>		// abort()
#include <string.h>

#ifdef LOGLIB_LOG4CXX
#include <log4cxx/logger.h>
#else
#include <iostream>
#endif	// LOGLIB_LOG4CXX

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
#ifdef LOGLIB_LOG4CXX
		static log4cxx::LoggerPtr GetLogger();
#endif	// LOGLIB_LOG4CXX
	private:
#ifdef LOGLIB_LOG4CXX
		static log4cxx::LoggerPtr logger;
#endif	// LOGLIB_LOG4CXX
		static std::atomic_bool isInit;
	};
}

#ifdef LOGLIB_LOG4CXX

#define LOG_TRACE(message) do {											\
			LOG4CXX_TRACE(tinyserver::Logger::GetLogger(), message);	\
		} while(0);

#define LOG_DEBUG(message) do {											\
			LOG4CXX_DEBUG(tinyserver::Logger::GetLogger(), message);	\
		} while(0);

#define LOG_INFO(message) do {											\
			LOG4CXX_INFO(tinyserver::Logger::GetLogger(), message);		\
		} while(0);

#define LOG_WARN(message) do {											\
			LOG4CXX_WARN(tinyserver::Logger::GetLogger(), message);		\
		} while(0);

#define LOG_ERROR(message) do {											\
			LOG4CXX_ERROR(tinyserver::Logger::GetLogger(), message);	\
		} while(0);

#define LOG_FATAL(message) do {											\
			LOG4CXX_FATAL(tinyserver::Logger::GetLogger(), message		\
					<< "[errno = " << errno << ", error string: " << strerror(errno) << "]");		\
		} while(0);

#else // else LOGLIB

#define LOG_TRACE(message) do {																									\
			std::cout << "[TRACE][" << __FILE__ << "](" << __LINE__  << ")] " << message << std::endl;							\
		} while(0);

#define LOG_DEBUG(message) do {																									\
			std::cout << "[DEBUG][" << __FILE__ << "](" << __LINE__  << ")] " << message << std::endl;							\
		} while(0);

#define LOG_INFO(message) do {																									\
			std::cout << "[INFO][" << __FILE__ << "](" << __LINE__  << ")] " << message << std::endl;							\
		} while(0);

#define LOG_WARN(message) do {																									\
			std::cout << "[WARN][" << __FILE__ << "](" << __LINE__  << ")] " << message << std::endl;							\
		} while(0);

#define LOG_ERROR(message) do {																									\
			std::cout << "[ERROR][" << __FILE__ << "](" << __LINE__  << ")] " << message << std::endl;							\
		} while(0);

#define LOG_FATAL(message) do {																									\
			std::cout << "[FATAL][" << __FILE__ << "](" << __LINE__  << ")] " << message << std::endl;							\
		} while(0);
#endif	// LOGLIB_LOG4CXX

#endif	// TINYSERVER_LOGGER_H
