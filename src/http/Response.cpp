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

void Response::reset()
{
	_status_code = 0;
	_status_msg.clear();
	_version.clear();
	_headers.clear();
	_body.clear();
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

bool Response::buildFromCgiOutput(std::vector<char> const& raw)
{
	if (raw.empty())
		return (false);

	std::string	data(raw.begin(), raw.end());
	size_t		sep;
	size_t		sepLen;
	size_t		crlf = data.find("\r\n\r\n");
	size_t		lf = data.find("\n\n");

	if (crlf != std::string::npos && (lf == std::string::npos || crlf <= lf))
	{
		sep = crlf;
		sepLen = 4;
	}
	else if (lf != std::string::npos)
	{
		sep = lf;
		sepLen = 2;
	}
	else
		return (false);

	std::string			headerBlock = data.substr(0, sep);
	std::string			body = data.substr(sep + sepLen);
	std::stringstream	headers(headerBlock);
	std::string			line;

	this->setStatusCode(200);
	this->setStatusMsg("OK");
	while (std::getline(headers, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		size_t	colon = line.find(':');
		if (colon == std::string::npos)
			continue ;
		std::string	key = line.substr(0, colon);
		std::string	value = line.substr(colon + 1);
		size_t		start = value.find_first_not_of(" \t");
		value = (start == std::string::npos) ? "" : value.substr(start);
		if (key == "Status")
		{
			std::stringstream	ss(value);
			int					code = 200;
			std::string			rest;

			ss >> code;
			std::getline(ss, rest);
			start = rest.find_first_not_of(" \t");
			this->setStatusCode(code);
			this->setStatusMsg(start == std::string::npos ? "" : rest.substr(start));
		}
		else
			this->addHeader(key, value);
	}

	std::vector<char>	bodyVec(body.begin(), body.end());
	std::stringstream	length;

	this->setBody(bodyVec);
	length << bodyVec.size();
	this->addHeader("Content-Length", length.str());
	return (true);
}


//**************CONSTRUCTOR/DESTRUCTOR******************//

Response::Response(Request const &request, Location &location) : _status_code(0), _location(&location)
{
	this->setVersion(request.getVersion());
	this->addHeader("connection", "keep-alive");
	if (request.getMethod() == "GET")
		handle_get(request);
	else if (request.getMethod() == "POST")
		handle_post(request);
	else if (request.getMethod() == "DELETE")
		handle_delete(request);
}

Response::Response(Location &location, std::string const& version) : _status_code(0), _location(&location)
{
	this->setVersion(version);
	this->addHeader("connection", "keep-alive");
}

Response::~Response()
{}

