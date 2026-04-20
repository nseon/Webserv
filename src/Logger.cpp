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
	char	date[11];
	time_t	time = std::time(NULL);
	std::tm	timeStruct;

	
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
