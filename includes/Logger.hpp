#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <fstream>
# include <string>

class Logger
{
	private:
		static Logger*	_loggerSingleton;

		std::ofstream	_outFile;


		Logger(void);
		Logger(std::string filename);

		static Logger*	getInstance(void);

		std::string		getDate(void);

	public:
		~Logger(void);

		static void	setInstance(std::string const& ofile);

		enum e_logLevel
		{
			NO_LEVEL,
			INFO,
			DEBUG,
			WARNING,
			ERROR,
			CRITICAL
		};

		static std::ostream&	log(enum e_logLevel level);
		static std::ostream&	info(void);
		static std::ostream&	debug(void);
		static std::ostream&	warning(void);
		static std::ostream&	error(void);
		static std::ostream&	critical(void);
};

#endif
