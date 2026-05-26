#include "cgi/CGIHandler.hpp"
#include <map>

std::pair<std::string, std::string> parsePathQuery(std::string& URI)
{
	std::pair<std::string, std::string>	ret;
	size_t								queryIndex = URI.find_first_of('?');

	if (queryIndex == std::string::npos)
	{
		ret.first = URI;
		ret.second = String();
	}
	else
	{
		ret.first = URI.substr(0, queryIndex);
		ret.second = URI.substr(queryIndex, URI.size());
	}
	return (ret);
}

bool	isCgi(std::string const & path,
		std::map<std::string, std::string>&	cgi_configs,
		std::string& cgi)
{
	std::map<std::string, std::string>::iterator	cgiIterator = cgi_configs.begin();
	std::map<std::string, std::string>::iterator	cgiEnd = cgi_configs.end();
	size_t											extension;

	while (cgiIterator != cgiEnd)
	{
		extension = path.rfind(cgiIterator->first);
		if (extension != std::string::npos && \
			(extension + cgiIterator->first.size() == path.size() || \
			path[extension + cgiIterator->first.size()] == '/'))
		{
			cgi = cgiIterator->second;
			return (true);
		}
		++cgiIterator;
	}
	return (false);
}

