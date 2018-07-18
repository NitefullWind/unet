#ifndef LOG_H
#define LOG_H

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>

#define LOG_TRACE(message) do {											\
		log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());	\
		LOG4CXX_TRACE(logger, message);									\
	} while(0);

#define LOG_DEBUG(message) do {											\
		log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());	\
		LOG4CXX_DEBUG(logger, message);									\
	} while(0);

#define LOG_INFO(message) do {											\
		log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());	\
		LOG4CXX_INFO(logger, message);									\
	} while(0);

#define LOG_WARN(message) do {											\
		log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());	\
		LOG4CXX_WARN(logger, message);									\
	} while(0);

#define LOG_ERROR(message) do {											\
		log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());	\
		LOG4CXX_ERROR(logger, message);									\
	} while(0);

#define LOG_FATAL(message) do {											\
		log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());	\
		LOG4CXX_FATAL(logger, message);									\
	} while(0);

#endif	// LOG_H
