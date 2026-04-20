#include "Logger.hpp"
#include <fstream>
#include <ostream>
#include <ctime>

Logger::Logger(void):
_outFile("webserv.log") {}

Logger::Logger(std::string filename):
_outFile(filename.c_str()) {}

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
}

Logger*	Logger::getInstance(void)
{
	return (_loggerSingleton);
}

void	Logger::setInstance(std::string const& ofile)
void	Logger::setInstance(std::ofstream const& ofile);

std::ostream&	info(void)
{

}
std::ostream&	debug(void)
{

}
std::ostream&	warning(void)
{

}
std::ostream&	error(void)
{

}
std::ostream&	critical(void)
{

}
