#ifndef CGI_HPP
# define CGI_HPP

# include <string>

# include "http/Request.hpp"
# include "config/Location.hpp"

struct CgiTarget
{
	std::string	interpreter;
	std::string	scriptName;
	std::string	pathInfo;
	std::string	queryString;
};

bool	parseCgiTarget(Location& location, std::string const& uri, CgiTarget& out);

char**	buildCgiEnvs(Request const& request, CgiTarget const& target, std::string const& remoteAddr);
void	freeCgiEnvs(char** envs);

int		checkExecutable(std::string const& path);

#endif
