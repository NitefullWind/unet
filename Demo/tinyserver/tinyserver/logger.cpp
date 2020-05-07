#include <tinyserver/logger.h>
#include <assert.h>

#ifdef LOGLIB_LOG4CXX
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#elif LOGLIB_SPDLOG
#include <iostream>
#include <spdlog/sinks/file_sinks.h>
#include <sys/stat.h>
#endif	// LOGLIB_LOG4CXX

using namespace tinyserver;

#ifdef LOGLIB_LOG4CXX
log4cxx::LoggerPtr Logger::logger = log4cxx::Logger::getRootLogger();
#endif	// LOGLIB_LOG4CXX
std::atomic_bool Logger::isInit;
Logger::Level Logger::_level = Level::Error;

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::SetLevel(Level level)
{
	Logger::_level = level;
#ifdef LOGLIB_SPDLOG
	spdlog::set_level(spdlog::level::level_enum(level));
#endif
}

#ifdef LOGLIB_LOG4CXX
log4cxx::LoggerPtr Logger::GetLogger()
{
	if(!Logger::isInit.load()) {					// 未进行初始化
		if(!Logger::isInit.exchange(true)) {		// 设为true并再次检查之前是否为false
			log4cxx::PatternLayoutPtr layout(new log4cxx::PatternLayout());
			layout->setConversionPattern("%d{yyyy-MM-dd HH:mm:ss,SSS}[%-5p][%t] %m %n");
			
			log4cxx::ConsoleAppenderPtr appender(new log4cxx::ConsoleAppender(layout));
			
			log4cxx::helpers::Pool p;
			appender->activateOptions(p);

			Logger::logger->addAppender(appender);
			Logger::logger->setLevel(log4cxx::Level::getTrace());
		}
	}
	return Logger::logger;
}
#else

void Logger::ConsoleLog(Level level, const std::stringstream& ss)
{
	if(level >= Logger::GetLevel()) {
		std::cout << ss.str();
	}
}
#endif	// LOGLIB_LOG4CXX

void Logger::Init(const char *filePath)
{
#ifdef LOGLIB_LOG4CXX
	assert(Logger::isInit.load() == false);
	Logger::isInit.exchange(true);
	log4cxx::PropertyConfigurator::configure(filePath);
#elif LOGLIB_SPDLOG
	try
	{
		struct stat buf;
		if(stat("logs", &buf)<0 || !S_ISDIR(buf.st_mode)) {
			if(mkdir("logs", S_IRWXU|S_IRGRP|S_IROTH) < 0) {
				std::cerr << "Failed to mkdir logs." << strerror(errno) << '\n';
			}
		}
		spdlog::drop_all();
		spdlog::set_level(spdlog::level::debug);
		spdlog::rotating_logger_mt("file_logger", "logs/tinyserver", 1024 * 1024 * 5, 10, true);
	}
	catch(const spdlog::spdlog_ex& ex)
	{
		std::cerr << "Log Initialization failed: " << ex.what() << '\n';
	}
#else
	SetLevel(Error);
#endif	// LOGLIB

}
