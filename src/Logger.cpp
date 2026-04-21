#include "Logger.hpp"
#include <fstream>
#include <ostream>
#include <ctime>
#include <iostream>

Logger::Logger(void) {}

Logger::~Logger(void) {}

std::string	Logger::getDate(void)
{
	char	date[23];
	time_t	timestamp;

    std::time(&timestamp);
    std::strftime(date, 23, "[%d %b %Y %H:%M:%S]", std::localtime(&timestamp));
	return (std::string(date));
}

std::ostream&	Logger::log(enum e_logLevel level)
{
	std::string	logLevelString;

	switch (level)
	{
		case NO_LEVEL: logLevelString = "NO_LVL";
		case INFO: logLevelString = "INFO";
		case DEBUG: logLevelString = "DEBUG";
		case WARNING: logLevelString = "WARNING";
		case ERROR: logLevelString = "ERROR";
		case CRITICAL: logLevelString = "CRITICAL";
	}
	std::cout << getDate << " [" << logLevelString << ']';
	return (std::cout);
}

Logger*	Logger::getInstance(void)
{
	if (!_loggerSingleton)
	{
		_loggerSingleton = new Logger();
	}
	return (_loggerSingleton);
}

std::ostream&		Logger::info(void)
{
	return (log(INFO));
}

std::ostream&		Logger::debug(void)
{
	return (log(DEBUG));
}

std::ostream&		Logger::warning(void)
{
	return (log(WARNING));
}
std::ostream&		Logger::error(void)
{
	return (log(ERROR));
}
std::ostream&		Logger::critical(void)
{
	return (log(CRITICAL));
}
