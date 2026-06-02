/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 13:34:54 by nseon             #+#    #+#             */
/*   Updated: 2026/05/25 15:36:39 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Response.hpp"
#include "http/responses.hpp"
#include "cgi/CGIHandler.hpp"
#include "socket/ClientSocket.hpp"
#include <fcntl.h>
#include <sstream>
#include <vector>

std::string Response::toString() const
{
	std::stringstream ss;

	ss << _status_code;
	std::string response = _version + " " + ss.str() + " " + _status_msg + "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
		response += it->first + ":" + it->second + "\r\n";
	response += "\r\n";
	response.insert(response.end(), _body.begin(), _body.end());
	return (response);
}

//**********************GETTER**************************//

Location Response::getLocation() const
{
	return (*_location);
}

int Response::getStatusCode() const
{
	return (_status_code);
}

//**********************SETTER**************************//

void Response::setStatusCode(int code)
{
	_status_code = code;
}

void Response::setStatusMsg(std::string msg)
{
	_status_msg = msg;
}

void Response::setBody(std::vector<char> body)
{
	_body = body;
}

//**************CGI*************************************//

// URI is the request path (without the local root): we extract
// QUERY_STRING, then split the remaining path into SCRIPT_NAME (up to and
// including the cgi script) and PATH_INFO (the extra path that follows).
// SCRIPT_NAME therefore never contains the local file root.
void Response::parseQueryString(std::string const& URI)
{
	std::string	path;
	size_t		queryIndex = URI.find_first_of('?');

	if (queryIndex == std::string::npos)
	{
		path = URI;
		_query_string = "";
	}
	else
	{
		path = URI.substr(0, queryIndex);
		_query_string = URI.substr(queryIndex + 1);
	}

	std::map<std::string, std::string>&				cgi = _location->getCgiConfigs();
	std::map<std::string, std::string>::iterator	it = cgi.begin();

	for (; it != cgi.end(); ++it)
	{
		size_t	extension = path.rfind(it->first);

		if (extension != std::string::npos && \
			(extension + it->first.size() == path.size() || \
			path[extension + it->first.size()] == '/'))
		{
			size_t	split = extension + it->first.size();

			_script_name = path.substr(0, split);
			_path_info = path.substr(split);
			return ;
		}
	}
	_script_name = path;
	_path_info = "";
}

bool	Response::isCgi(std::string const& URI)
{
	std::map<std::string, std::string>::iterator	cgiIterator = this->_location->getCgiConfigs().begin();
	std::map<std::string, std::string>::iterator	cgiEnd = this->_location->getCgiConfigs().end();
	size_t											extension;

	while (cgiIterator != cgiEnd)
	{
		extension = URI.rfind(cgiIterator->first);
		if (extension != std::string::npos && \
			(extension + cgiIterator->first.size() == URI.size() || \
			URI[extension + cgiIterator->first.size()] == '/'))
		{
			this->_cgi = cgiIterator->second;
			return (true);
		}
		++cgiIterator;
	}
	return (false);
}


//**************CONSTRUCTOR/DESTRUCTOR******************//

Response::Response(Request const &request, Location &location, ClientSocket& client) : _status_code(0), _location(&location), _client(client)
{
	this->setVersion(request.getVersion());
	this->addHeader("connection", "keep-alive");
	if (request.getMethod() == "GET")
		handle_get(request, this);
	else if (request.getMethod() == "POST")
		handle_post(request, this);
	else if (request.getMethod() == "DELETE")
		handle_delete(request, this);
}

Response::~Response()
{}

