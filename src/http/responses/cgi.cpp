#include "http/Request.hpp"
#include "http/Response.hpp"
#include <string>
#include <vector>
#include <fcntl.h>

// TODO: Environment variables to implement (MANDATORY)
//  AUTH_TYPE
//  CONTENT_LENGTH (if request has a body)
//  CONTENT_TYPE (if request has Content-Type header)
//  GATEWAY_INTERFACE
//  QUERY_STRING
//  REMOTE_ADDR
//  REQUEST_METHOD
// SCRIPT_NAME
// SERVER_NAME
// SERVER_PORT
// SERVER_PROTOCOL
// SERVER_SOFTWARE
//
//
// (GOOD OPTIONAL)
// PATH_INFO
//
// (I DONT CARE OPTIONAL)
// PATH_TRANSLATED

bool	Response::isExec(std::string const& path)
{
	struct stat	st;

	if (stat(path.c_str(), &st))
		return (error(response, 404, "Not found"));
	if (!st.st_mode & X_OK)
	{
		return (error(response, 403, "Forbidden"));
	}
	return (false);
}

namespace
{

void	setAuthType(std::vector<std::string>& envs)
{
	envs.push_back("AUTH_TYPE=");
}

void	setContentLength(std::vector<std::string>& envs, Request const& request)
{
	std::string										contentLength;
	std::map<std::string, std::string>::iterator	it = request.getHeaders().find("Content-Length");

	if (it != request.getHeaders().end())
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
	std::map<std::string, std::string>::iterator	it = request.getHeaders().find("Content-Type");

	if (it != request.getHeaders().end())
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

}

char**	Response::createCgiEnvs(std::pair<std::string, std::string> const& pathQuery, Request const& request)
{
	std::vector<std::string>	envs(12);

	setAuthType(envs);
	setContentLength(envs, request);
	setContentType(envs, request);
	setGatewayInterface(envs);
	setQueryString(envs, pathQuery.second);
	setRemoteAddress(envs, this->_client.getAddress());
	serRequestMethod(envs, request.getMethod());
}

int Response::handle_cgi(std::pair<std::string, std::string> const& pathQuery, int mode)
{
	int	ret;

	ret = this->isExec(pathQuery.first);
	if (ret)
	{
		return (ret);
	}

	char	**cgiEnvs;
}

