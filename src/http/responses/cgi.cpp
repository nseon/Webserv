#include "http/Request.hpp"
#include "http/Response.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <fcntl.h>
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
	std::map<std::string, std::string>::iterator	it = headers.find("Content-Length");

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
	std::map<std::string, std::string>::iterator	it = headers.find("Content-Type");

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

char**	Response::createCgiEnvs(Request const& request)
{
	std::vector<std::string>	envs;

	setAuthType(envs);
	setContentLength(envs, request);
	setContentType(envs, request);
	setGatewayInterface(envs);
	setQueryString(envs, this->_query_string);
	setRemoteAddress(envs, this->_client.getAddress());
	setRequestMethod(envs, request.getMethod());
	setScriptName(envs, this->_script_name);
	setPathInfo(envs, this->_path_info);
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

int	Response::isExec(std::string const& URI)
{
	struct stat	st;

	if (stat(URI.c_str(), &st))
		return (this->error(404, "Not found"));
	if (!(st.st_mode & X_OK))
	{
		return (this->error(403, "Forbidden"));
	}
	return (0);
}

int Response::handle_cgi(Request const& request, int mode)
{
	std::string	scriptPath = this->getLocation().getRoot() + this->_script_name;
	int			ret;

	ret = this->isExec(scriptPath);
	if (ret)
	{
		return (ret);
	}
	char	**cgiEnvs = this->createCgiEnvs(request);

	(void)cgiEnvs;
	(void)mode;
	return (0);
}

