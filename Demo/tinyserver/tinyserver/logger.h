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
		static void ConsoleLog(Level level, std::stringstream&& ss);
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
			std::stringstream ss; ss << message;			\
			spdlog::get("file_logger")->trace(ss.str());	\
		} while(0);

#define TLOG_DEBUG(message) do {							\
			std::stringstream ss; ss << message;			\
			spdlog::get("file_logger")->debug(ss.str());	\
		} while(0);

#define TLOG_INFO(message) do {								\
			std::stringstream ss; ss << message;			\
			spdlog::get("file_logger")->info(ss.str());		\
		} while(0);

#define TLOG_WARN(message) do {								\
			std::stringstream ss; ss << message;			\
			spdlog::get("file_logger")->warn(ss.str());		\
		} while(0);

#define TLOG_ERROR(message) do {							\
			std::stringstream ss; ss << message;			\
			spdlog::get("file_logger")->error(ss.str());	\
		} while(0);

#define TLOG_FATAL(message) do {							\
			std::stringstream ss; ss << message;			\
			spdlog::get("file_logger")->critical(ss.str());	\
		} while(0);

#else // else LOGLIB

#define TLOG_TRACE(message) do {									\
			std::stringstream ss; ss << message << '\n';			\
			Logger::ConsoleLog(Logger::Trace, std::move(ss));		\
		} while(0);

#define TLOG_DEBUG(message) do {									\
			std::stringstream ss; ss << message << '\n';			\
			Logger::ConsoleLog(Logger::Debug, std::move(ss));		\
		} while(0);

#define TLOG_INFO(message) do {										\
			std::stringstream ss; ss << message << '\n';			\
			Logger::ConsoleLog(Logger::Info, std::move(ss));		\
		} while(0);

#define TLOG_WARN(message) do {										\
			std::stringstream ss; ss << message << '\n';			\
			Logger::ConsoleLog(Logger::Warn, std::move(ss));		\
		} while(0);

#define TLOG_ERROR(message) do {									\
			std::stringstream ss; ss << message << '\n';			\
			Logger::ConsoleLog(Logger::Error, std::move(ss));		\
		} while(0);

#define TLOG_FATAL(message) do {									\
			std::stringstream ss; ss << message << '\n';			\
			Logger::ConsoleLog(Logger::Critial, std::move(ss));		\
		} while(0);
#endif	// LOGLIB

#endif	// TINYSERVER_LOGGER_H
