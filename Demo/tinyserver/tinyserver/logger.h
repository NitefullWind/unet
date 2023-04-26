#ifndef TINYSERVER_LOGGER_H
#define TINYSERVER_LOGGER_H

#include <atomic>		// atomic_bool
#include <cstdlib>		// abort()
#include <string.h>

#ifdef LOGLIB_LOG4CXX
#include <log4cxx/logger.h>
#elif LOGLIB_SPDLOG
#include <sstream>
#include <spdlog/spdlog.h>
#else
#include <iostream>
#include <sstream>
#endif	// LOGLIB_LOG4CXX

namespace tinyserver
{
	/**
	 * 日志封装类
	 * 也可以调用Init()函数初始化日志库
	 */
	class Logger
	{
	public:
		enum Level {
			Trace,
			Debug,
			Info,
			Warn,
			Error,
			Critial,
			Off
		};

		Logger();
		~Logger();

		static void Init(const char *filePath = nullptr);
		static void SetLevel(Level level);
		static Level GetLevel() { return Logger::_level; }
#ifdef LOGLIB_LOG4CXX
		static log4cxx::LoggerPtr GetLogger();
#else
		static void ConsoleLog(Level level, const std::stringstream& ss);
#endif	// LOGLIB
	private:
#ifdef LOGLIB_LOG4CXX
		static log4cxx::LoggerPtr logger;
#endif	// LOGLIB_LOG4CXX
		static std::atomic_bool isInit;
		static Level _level;
	};
}

#ifdef LOGLIB_LOG4CXX

#define TLOG_TRACE(message) do {											\
			LOG4CXX_TRACE(tinyserver::Logger::GetLogger(), message);	\
		} while(0);

#define TLOG_DEBUG(message) do {											\
			LOG4CXX_DEBUG(tinyserver::Logger::GetLogger(), message);	\
		} while(0);

#define TLOG_INFO(message) do {											\
			LOG4CXX_INFO(tinyserver::Logger::GetLogger(), message);		\
		} while(0);

#define TLOG_WARN(message) do {											\
			LOG4CXX_WARN(tinyserver::Logger::GetLogger(), message);		\
		} while(0);

#define TLOG_ERROR(message) do {											\
			LOG4CXX_ERROR(tinyserver::Logger::GetLogger(), message);	\
		} while(0);

#define TLOG_FATAL(message) do {											\
			LOG4CXX_FATAL(tinyserver::Logger::GetLogger(), message		\
					<< "[errno = " << errno << ", error string: " << strerror(errno) << "]");		\
		} while(0);
#elif LOGLIB_SPDLOG
#define TLOG_TRACE(message) do {							\
			std::stringstream __tlog_ss__; __tlog_ss__ << message;			\
			spdlog::get("file_logger")->trace(__tlog_ss__.str());	\
		} while(0);

#define TLOG_DEBUG(message) do {							\
			std::stringstream __tlog_ss__; __tlog_ss__ << message;			\
			spdlog::get("file_logger")->debug(__tlog_ss__.str());	\
		} while(0);

#define TLOG_INFO(message) do {								\
			std::stringstream __tlog_ss__; __tlog_ss__ << message;			\
			spdlog::get("file_logger")->info(__tlog_ss__.str());		\
		} while(0);

#define TLOG_WARN(message) do {								\
			std::stringstream __tlog_ss__; __tlog_ss__ << message;			\
			spdlog::get("file_logger")->warn(__tlog_ss__.str());		\
		} while(0);

#define TLOG_ERROR(message) do {							\
			std::stringstream __tlog_ss__; __tlog_ss__ << "[" << __FILE__ << ":" << __LINE__ << "] " << message;			\
			spdlog::get("file_logger")->error(__tlog_ss__.str());	\
		} while(0);

#define TLOG_FATAL(message) do {							\
			std::stringstream __tlog_ss__; __tlog_ss__ << "[" << __FILE__ << ":" << __LINE__ << "] " << message;			\
			spdlog::get("file_logger")->critical(__tlog_ss__.str());	\
		} while(0);

#else // else LOGLIB

#define TLOG_TRACE(message) do {									\
			std::stringstream __tlog_ss__; __tlog_ss__ << message << '\n';			\
			tinyserver::Logger::ConsoleLog(tinyserver::Logger::Trace, __tlog_ss__);		\
		} while(0);

#define TLOG_DEBUG(message) do {									\
			std::stringstream __tlog_ss__; __tlog_ss__ << message << '\n';			\
			tinyserver::Logger::ConsoleLog(tinyserver::Logger::Debug, __tlog_ss__);		\
		} while(0);

#define TLOG_INFO(message) do {										\
			std::stringstream __tlog_ss__; __tlog_ss__ << message << '\n';			\
			tinyserver::Logger::ConsoleLog(tinyserver::Logger::Info, __tlog_ss__);		\
		} while(0);

#define TLOG_WARN(message) do {										\
			std::stringstream __tlog_ss__; __tlog_ss__ << message << '\n';			\
			tinyserver::Logger::ConsoleLog(tinyserver::Logger::Warn, __tlog_ss__);		\
		} while(0);

#define TLOG_ERROR(message) do {									\
			std::stringstream __tlog_ss__; __tlog_ss__ << "[" << __FILE__ << ":" << __LINE__ << "] " << message << '\n';			\
			tinyserver::Logger::ConsoleLog(tinyserver::Logger::Error, __tlog_ss__);		\
		} while(0);

#define TLOG_FATAL(message) do {									\
			std::stringstream __tlog_ss__; __tlog_ss__ << "[" << __FILE__ << ":" << __LINE__ << "] " << message << '\n';			\
			tinyserver::Logger::ConsoleLog(tinyserver::Logger::Critial, __tlog_ss__);		\
		} while(0);
#endif	// LOGLIB

#endif	// TINYSERVER_LOGGER_H
