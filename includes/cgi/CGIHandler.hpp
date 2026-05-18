#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include <map>
# include <string>

bool	isCgi(std::string URI, std::map<std::string, std::string> cgi_configs);

#endif
