#include "http/cgi.hpp"
#include "http/Request.hpp"
#include "config/Location.hpp"
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

namespace
{

void	setAuthType(std::vector<std::string>& envs)
{
	envs.push_back("AUTH_TYPE=");
}

void	setContentLength(std::vector<std::string>& envs, Request const& request)
{
	std::string										contentLength;
	std::map<std::string, std::string>				headers = request.getHeaders();
	std::map<std::string, std::string>::iterator	it = headers.find("content-length");

	if (it != headers.end())
	{
		contentLength = "CONTENT_LENGTH=" + it->second;
	}
	else
	{
		contentLength = "CONTENT_LENGTH=";
	}
	envs.push_back(contentLength);
}

void	setContentType(std::vector<std::string>& envs, Request const& request)
{
	std::string										contentLength;
	std::map<std::string, std::string>				headers = request.getHeaders();
	std::map<std::string, std::string>::iterator	it = headers.find("content-type");

	if (it != headers.end())
	{
		contentLength = "CONTENT_TYPE=" + it->second;
	}
	else
	{
		contentLength = "CONTENT_TYPE=";
	}
	envs.push_back(contentLength);
}

void	setGatewayInterface(std::vector<std::string>& envs)
{
	envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
}

void	setQueryString(std::vector<std::string>& envs, std::string const& queryString)
{
	envs.push_back("QUERY_STRING=" + queryString);
}

void	setRemoteAddress(std::vector<std::string>& envs, std::string addr)
{
	envs.push_back("REMOTE_ADDR=" + addr);
}

void	setRequestMethod(std::vector<std::string>& envs, std::string method)
{
	envs.push_back("REQUEST_METHOD=" + method);
}

void	setScriptName(std::vector<std::string>& envs, std::string scriptName)
{
	envs.push_back("SCRIPT_NAME=" + scriptName);
}

void	setPathInfo(std::vector<std::string>& envs, std::string pathInfo)
{
	envs.push_back("PATH_INFO=" + pathInfo);
}

void	setServerName(std::vector<std::string>& envs, std::string serverName)
{
	envs.push_back("SERVER_NAME=" + serverName);
}

void	setServerPort(std::vector<std::string>& envs, unsigned int port)
{
	std::stringstream	ss;

	ss << port;
	envs.push_back("SERVER_PORT=" + ss.str());
}

void	setServerProtocol(std::vector<std::string>& envs)
{
	envs.push_back("SERVER_PROTOCOL=HTTP/1.0");
}

void	setServerSoftware(std::vector<std::string>& envs)
{
	envs.push_back("SERVER_SOFTWARE=webserv");
}

}

char**	buildCgiEnvs(Request const& request, CgiTarget const& target, std::string const& remoteAddr)
{
	std::vector<std::string>	envs;

	setAuthType(envs);
	setContentLength(envs, request);
	setContentType(envs, request);
	setGatewayInterface(envs);
	setQueryString(envs, target.queryString);
	setRemoteAddress(envs, remoteAddr);
	setRequestMethod(envs, request.getMethod());
	setScriptName(envs, target.scriptName);
	setPathInfo(envs, target.pathInfo);
	setServerName(envs, request.getServer()->getAddr());
	setServerPort(envs, request.getServer()->getPort());
	setServerProtocol(envs);
	setServerSoftware(envs);

	char**	ret = new char*[envs.size() + 1];

	for (size_t i = 0; i < envs.size(); ++i)
	{
		ret[i] = new char[envs[i].size() + 1];
		std::strcpy(ret[i], envs[i].c_str());
	}
	ret[envs.size()] = NULL;
	return (ret);
}

void	freeCgiEnvs(char** envs)
{
	if (!envs)
		return ;
	for (size_t i = 0; envs[i]; ++i)
		delete[] envs[i];
	delete[] envs;
}

int	checkExecutable(std::string const& path)
{
	struct stat	st;

	if (stat(path.c_str(), &st))
		return (404);
	if (!S_ISREG(st.st_mode) || access(path.c_str(), X_OK))
		return (403);
	return (0);
}

bool	parseCgiTarget(Location& location, std::string const& uri, CgiTarget& out)
{
	std::string	path;
	size_t		queryIndex = uri.find_first_of('?');

	if (queryIndex == std::string::npos)
	{
		path = uri;
		out.queryString = "";
	}
	else
	{
		path = uri.substr(0, queryIndex);
		out.queryString = uri.substr(queryIndex + 1);
	}

	std::map<std::string, std::string>&				cgi = location.getCgiConfigs();
	std::map<std::string, std::string>::iterator	it = cgi.begin();

	for (; it != cgi.end(); ++it)
	{
		size_t	extension = path.rfind(it->first);

		if (extension != std::string::npos && \
			(extension + it->first.size() == path.size() || \
			path[extension + it->first.size()] == '/'))
		{
			size_t	split = extension + it->first.size();

			out.scriptName = path.substr(0, split);
			out.pathInfo = path.substr(split);
			out.interpreter = it->second;
			return (true);
		}
	}
	return (false);
}

