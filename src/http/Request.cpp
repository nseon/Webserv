/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 14:01:57 by nseon             #+#    #+#             */
/*   Updated: 2026/05/06 10:44:57 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

#include "Request.hpp"

//***********************PARSING**************************//

static void trim(std::string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
	
	if (first == std::string::npos)
		str = "";
	else
	{
		size_t last = str.find_last_not_of(" \t\r\n");
	
    	str = str.substr(first, (last - first + 1));
	}
}

static void parse_request_line(Request &request, std::string request_line)
{
	std::stringstream ss(request_line);
	std::string token;

	for (int i = 0; i < 3; ++i)
	{
		if (ss >> token)
		{
			if (i == 0)
				request.setMethod(token);
			else if (i == 1)
				request.setPath(token);
			else if (i == 2)
				request.setVersion(token);
		}
		else
			throw std::logic_error("Invalid request line: " + request_line);
	}
	if (ss >> token)
			throw std::logic_error("Invalid request line: " + request_line);
}

static void parse_headers(Request &request, std::string line)
{
	while (line.size())
	{
		size_t pos = line.find("\r\n");
		
		if (pos == std::string::npos)
			throw std::logic_error("Invalid header in request: " + line);

		std::string header = line.substr(0, pos);
		
		line.erase(0, pos + 2);
		if (header.empty())
            continue;
		pos = header.find_first_of(':');
		if (pos == std::string::npos)
			throw std::logic_error("Invalid header in request: " + header);
		
		std::string key = header.substr(0, pos);
		std::string value = header.substr(pos + 1);
		
		trim(value);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		request.addHeader(key, value);
	}
}

bool Request::_parseBodyContent(std::map<std::string, std::string>::iterator &i)
{
	size_t body_length = std::atoi(i->second.c_str());
	size_t bytes_needed = body_length - _body.size();
	size_t bytes_to_copy = std::min(bytes_needed, _raw_data.size());
	
	_body.insert(_body.end(), _raw_data.begin(), _raw_data.begin() + bytes_to_copy);
	_raw_data.erase(0, bytes_to_copy);
	if (_body.size() == body_length)
	{
		_parsing_state = DONE;
		return (true);
	}
	else
		return (false);
}

bool Request::_parseBodyChunked()
{
	while (true)
	{
		if (_chunk_state == CHUNK_SIZE)
		{
			size_t pos = _raw_data.find(("\r\n"));
			
			if (pos == std::string::npos)
				return (false);
			
			std::string size_hex = _raw_data.substr(0, pos);
			std::stringstream ss;

			ss << std::hex << size_hex;
			if (!(ss >> _chunk_size))
				throw std::logic_error("Invalid chunk size: " + size_hex);
			_raw_data.erase(0, pos + 2);
			if (_chunk_size == 0)
			{
				_parsing_state = TRAILERS;
				return (true);
			}
			_chunk_state = CHUNK_DATA;
		}
		if (_chunk_state == CHUNK_DATA)
		{
			if (_raw_data.size() < _chunk_size)
				return (false);
			_body.insert(_body.end(), _raw_data.begin(), _raw_data.begin() + _chunk_size);
			_raw_data.erase(0, _chunk_size);
			_chunk_state = CHUNK_CRLF;
		}
		if (_chunk_state == CHUNK_CRLF)
		{
			if (_raw_data.size() < 2)
				return (false);
			if (_raw_data.substr(0, 2) != "\r\n")
				throw std::logic_error("Invalid chunk");
			_raw_data.erase(0, 2);
			_chunk_state = CHUNK_SIZE;
		}
	}
}

bool Request::_handleRequestLine()
{
	size_t pos = _raw_data.find("\r\n");

	if (pos != std::string::npos)
	{
		parse_request_line(*this, _raw_data.substr(0, pos));
		_raw_data.erase(0, pos + 2);
		_parsing_state = HEADERS;
		return (true);
	}
	return (false);
}

bool Request::_handleHeaders()
{
	size_t pos = _raw_data.find("\r\n\r\n");
		
	if (pos != std::string::npos)
	{
		parse_headers(*this, _raw_data.substr(0, pos + 2));
		_raw_data.erase(0, pos + 4);
		if (_parsing_state == TRAILERS)
			_parsing_state = DONE;
		else
			_parsing_state = BODY;
		return (true);
	}
	return (false);
}

bool Request::_handleBody()
{
	
	std::map<std::string, std::string>::iterator i =_headers.find("transfer-encoding");
	
	if (i != _headers.end())
	{
		if (i->second != "chunked")
			throw std::logic_error("Transfer encoding non supported: " + i->second);
		else
		 	return (_parseBodyChunked());
	}

	i =_headers.find("content-length");

	if (i != _headers.end())
		return(_parseBodyContent(i));
	else
	{
		_parsing_state = DONE;
		return (true);
	}
}

bool Request::parseRequest(std::string msg)
{
	bool progress = true;
	_raw_data += msg;
	
	while (progress)
	{
		if (_parsing_state == REQUEST)
			progress = _handleRequestLine();
		if (_parsing_state == HEADERS || _parsing_state == TRAILERS)
			progress = _handleHeaders();
		if (_parsing_state == BODY)
			progress = _handleBody();
		if (_parsing_state == DONE)
			return (true);
	}
	return (false);
}

//**************CONSTRUCTOR/DESTRUCTOR******************//

Request::Request() : _parsing_state(REQUEST), _chunk_state(CHUNK_SIZE), _chunk_size(0)
{}

Request::~Request()
{}

void Request::reset()
{
	_parsing_state = REQUEST;
	_chunk_state = CHUNK_SIZE;
	_chunk_size = 0;
	_raw_data.clear();
	_method.clear();
	_path.clear();
	_version.clear();
	_headers.clear();
	_body.clear();
}

//***********************PRINT**************************//

std::ostream &operator<<(std::ostream &os, const Request &req)
{
    os << "---------- Request View ----------" << std::endl;
    
    // Affichage de la Request Line
    os << "Method  : " << req.getMethod() << std::endl;
    os << "Path    : " << req.getPath() << std::endl;
    os << "Version : " << req.getVersion() << std::endl;
    
    os << "Headers : " << std::endl;

    // Itération sur la map des headers (Syntaxe C++98)
    std::map<std::string, std::string>::const_iterator it;
    const std::map<std::string, std::string> &headers = req.getHeaders();
    
    if (headers.empty()) {
        os << "  (none)" << std::endl;
    } else {
        for (it = headers.begin(); it != headers.end(); ++it) {
            os << "  [" << it->first << "]: " << it->second << std::endl;
        }
    }
    os << "Body : ";
	std::vector<char> body = req.getBody();
	std::copy(body.begin(), body.end(), std::ostream_iterator<char>(std::cout));
	os << std::endl;
    os << "----------------------------------";
    return os;
}

//**********************GETTER**************************//

int Request::getParsingState() const
{
	return (_parsing_state);
}

std::string Request::getRawData() const
{
	return (_raw_data);
}

std::string Request::getMethod() const
{
	return (_method);
}

std::string Request::getPath() const
{
	return (_path);
}

std::string Request::getVersion() const
{
	return (_version);
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return (_headers);
}

std::vector<char> Request::getBody() const
{
	return (_body);
}

//**********************SETTER**************************//

void Request::setMethod(std::string const &value)
{
	_method = value;
}

void Request::setPath(std::string const &value)
{
	_path = value;
}		

void Request::setVersion(std::string const &value)
{
	_version = value;
}

void Request::addHeader(std::string const &key, std::string const &value)
{
	_headers[key] = value;
}
