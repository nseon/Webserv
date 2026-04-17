#include "Logger.hpp"
#include <fstream>
#include <ostream>


Logger::Logger(void) {}

Logger::Logger(std::string filename)
{
	_outFile.open(filename.c_str());
}

Logger::Logger(std::ofstream& file)
{
	_outFile = file;
}

Logger::~Logger(void) {}

static std::ostream&	log(enum e_logLevel level)
{
	std::ostream	ret;

	std::cout << ret;
	return (ret);
}

static std::ostream&	info(void);
static std::ostream&	debug(void);
static std::ostream&	warning(void);
static std::ostream&	error(void);
static std::ostream&	critical(void);
