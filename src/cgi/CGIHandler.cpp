#include "cgi/CGIHandler.hpp"
#include <map>

bool	isCgi(std::string URI,
		std::map<std::string, std::string>	cgi_configs)
{
	std::map<std::string, std::string>::iterator	cgiIterator = cgi_configs.begin();
	std::map<std::string, std::string>::iterator	cgiEnd = cgi_configs.end();
	size_t											extension;

	while (cgiIterator != cgiEnd)
	{
		extension = URI.rfind(cgiIterator->first);
		if (extension != std::string::npos && \
			(extension + cgiIterator->first.size() == URI.size() || \
			URI[extension + cgiIterator->first.size()] == '/'))
		{
			return (true);
		}
		++cgiIterator;
	}
	return (false);
}
