#include <muduo/base/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <assert.h>

using namespace muduo;

log4cxx::LoggerPtr Logger::logger = log4cxx::Logger::getRootLogger();
std::atomic_bool Logger::isInit;

Logger::Logger()
{
}

Logger::~Logger()
{
}

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

void Logger::InitByFile(const char *filePath)
{
	assert(Logger::isInit.load() == false);
	Logger::isInit.exchange(true);
	log4cxx::PropertyConfigurator::configure(filePath);
}
