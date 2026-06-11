/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 13:34:54 by nseon             #+#    #+#             */
/*   Updated: 2026/06/10 16:46:02 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Response.hpp"
#include "http/responses.hpp"
#include "http/errors.hpp"
#include <fcntl.h>
#include <sstream>
#include <vector>

void Response::reset()
{
	_version.clear();
	_headers.clear();
	_body.clear();
	_status_code = 0;
	_status_msg.clear();
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
		return "text/text/css";
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
		case 505: return "HTTP Version Not Supported";
		default:  return "Unknown Status";
	}
}

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

void Response::handle_redirection()
{
	if (getLocation()->getReturn().first != 0)
		setStatusCode(getLocation()->getReturn().first);
	else
	 	setStatusCode(301);
	setStatusMsg(getStatusMessage(getStatusCode()));
	addHeader("Location", getLocation()->getReturn().second);
	addHeader("Content-Length", "0");
}

//**********************GETTER**************************//

Location *Response::getLocation() const
{
	return (_location);
}

Request *Response::getRequest() const
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

//**************CONSTRUCTOR/DESTRUCTOR******************//

Response::Response(int error_code, Server &server) : _status_code(error_code), _location(NULL), _request(NULL)
{
	std::string msg = Response::getStatusMessage(error_code);

	setVersion("HTTP/1.1");
	addHeader("connection", "keep-alive");
	
	std::string error_path = server.getErrorPath(error_code);
	
	if (error_path.empty())
	{
		std::stringstream ss;
		
		setBody(generate_default_error_page(error_code, msg));
		ss << getBody().size();
		addHeader("Content-Length", ss.str());
	}
	else if (getRessource(error_path, *this))
	{
		std::stringstream ss;
		
		setBody(generate_default_error_page(error_code, msg));
		ss << getBody().size();
		addHeader("Content-Length", ss.str());
	}
}

Response::Response(Request &request, Location *location) : _status_code(0), _location(location), _request(&request)
{
	setVersion("HTTP/1.1");
	addHeader("connection", "keep-alive");
	if (!getLocation())
		fill_error(*this, 404);
	else if (!getLocation()->getReturn().second.empty())
		handle_redirection();
	else if (request.getMethod() == "GET")
		handle_get(request, *this);
	else if (request.getMethod() == "POST")
		handle_post(request, *this);
	else if (request.getMethod() == "DELETE")
		handle_delete(request, *this);
}

Response::~Response()
{}

