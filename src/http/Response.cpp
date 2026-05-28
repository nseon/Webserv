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

std::pair<std::string, std::string> Response::parsePathQuery(std::string const& URI)
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

bool	Response::isCgi(std::string const% URI)
{
	std::map<std::string, std::string>::iterator	cgiIterator = this->location_->getCgiConfigs().begin();
	std::map<std::string, std::string>::iterator	cgiEnd = this->location_->getCgiConfigs().end();
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


//**************CONSTRUCTOR/DESTRUCTOR******************//

Response::Response(Request const &request, Location &location) : _status_code(0), _location(&location)
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

