/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 13:34:54 by nseon             #+#    #+#             */
/*   Updated: 2026/06/18 16:46:56 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Response.hpp"
#include "http/Request.hpp"
#include "http/responses.hpp"
#include <cstddef>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

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

std::string Response::getContentType(std::string const &path)
{
	size_t pos = path.find_last_of('.');

	if (pos == std::string::npos)
		return "text/html";

	std::string ext = path.substr(pos);

	if (ext == ".html" || ext == ".htm")
		return "text/html";
	if (ext == ".css")
		return "text/css";
	if (ext == ".ico")
		return "image/x-icon";
	if (ext == ".svg")
		return "image/svg+xml";
	if (ext == ".png")
		return "image/png";
	if (ext == ".jpg" || ext == ".jpeg")
		return "image/jpeg";

	return "text/plain";
}

std::string Response::getStatusMessage(int code)
{
	switch (code)
	{
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 413: return "Payload Too Large";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway Error";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		default:  return "Unknown Status";
	}
}

void Response::handle_redirection()
{
	if (getLocation().getReturn().first != 0)
		setStatusCode(getLocation().getReturn().first);
	else
		setStatusCode(301);
	setStatusMsg(getStatusMessage(getStatusCode()));
	addHeader("Location", getLocation().getReturn().second);
	addHeader("Content-Length", "0");
}

//**********************GETTER**************************//

Location const& Response::getLocation() const
{
	return (*_location);
}

Request const *Response::getRequest() const
{
	return (_request);
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

void Response::setBody(std::string const &str)
{
	_body.assign(str.begin(), str.end());
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

Response::Response(Request const &request, Location *location, int socketfd)
	: _status_code(0), _location(location), _request(&request)
{
	std::map<std::string, std::string>::const_iterator i = getRequest()->getHeaders().find("Cookie");
	
	if (i == getRequest()->getHeaders().end())
	{
		std::stringstream ss;

		ss << socketfd << "_" << std::time(NULL);
		this->addHeader("Set-Cookie", "id=" + ss.str());
	}
	this->setVersion(request.getVersion());
	this->addHeader("connection", "keep-alive");
	if (!_location)
		this->error(404);
	else if (!getLocation().getReturn().second.empty())
		handle_redirection();
	else if (request.getMethod() == "GET")
		handle_get(request);
	else if (request.getMethod() == "POST")
		handle_post(request);
	else if (request.getMethod() == "DELETE")
		handle_delete(request);
}

Response::Response(Location &location, std::string const& version, Request const& request)
	: _status_code(0), _location(&location), _request(&request)
{
	this->setVersion(version);
	this->addHeader("connection", "keep-alive");
}

Response::Response(int code, Server &server)
	: _status_code(code), _location(NULL), _request(NULL)
{
	std::string	msg = getStatusMessage(code);

	this->setVersion("HTTP/1.1");
	this->addHeader("connection", "keep-alive");
	this->setStatusMsg(msg);

	std::string	error_path = server.getErrorPath(code);

	if (error_path.empty() || getRessource(error_path, *this) != 200)
	{
		std::stringstream	ss;

		this->setBody(generate_default_error_page(code, msg));
		ss << this->getBody().size();
		this->addHeader("Content-Length", ss.str());
	}
	this->setStatusCode(code);
	this->setStatusMsg(msg);
}

Response::~Response()
{}
