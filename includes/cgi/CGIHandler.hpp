#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <string>

class CGIHandler
{
	private:
		char**	_envs;
	public:
		CGIHandler(void);
		~CGIHandler(void);

		bool	isCgi(std::string URI);
		void	setEnvs(std::string URI);
};

#endif
