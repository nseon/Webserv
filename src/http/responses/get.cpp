/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:01:26 by nseon             #+#    #+#             */
/*   Updated: 2026/05/26 14:41:29 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "cgi/CGIHandler.hpp""
#include "http/Response.hpp"
#include "http/errors.hpp"
#include <cstddef>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <iostream>

void handle_directory()
{}

int getRessource(std::string const &path, Response &response)
{
	struct stat st;

	if (stat(path.c_str(), &st))
		return (404);
	else if (S_ISDIR(st.st_mode))
	{
		handle_directory();
		return (1);
	}
	else if (!S_ISREG(st.st_mode) || !(st.st_mode & S_IRUSR))
		return (403);
	else
	{
		std::stringstream ss;

		ss << st.st_size;
		
		std::ifstream file(path.c_str(), std::ios::binary);
		
		if (!file)
			return (500);
		
		std::vector<char> content = std::vector<char>((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
		
		response.setBody(content);
		response.addHeader("Content-Length", ss.str());
		response.setStatusCode(200);
		response.setStatusMsg("OK");
	}
	return (0);
}

int getFile(std::string const &path, Response &response)
{
	switch (getRessource(path, response))
	{
		case 404: return (error(response, 404, "Not found"));
		case 403: return (error(response, 403, "Forbidden"));
		case 500: return (error(response, 500, "Internal Server Error"));
	}
	return (0);
}

// TODO: Environment variables to implement (MANDATORY)
// AUTH_TYPE
// CONTENT_LENGTH (if request has a body)
// CONTENT_TYPE (if request has Content-Type header)
// GATEWAY_INTERFACE
// QUERY_STRING
// REMOTE_ADDR
// REQUEST_METHOD
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

int handle_cgi(std::pair<std::string, std::string> const& pathQuery)
{
	
}

int Response::handle_get(Request const &request)
{
	if (!this->getLocation().getAllowGet())
		return (error(*this, 405, "Method not allowed"));
	this->setStatusCode(200);
	this->setStatusMsg("OK");

	std::string URI= this->getLocation().getRoot() + request.getPath();
	std::pair<std::string, std::string> pathQuery = parsePathQuery(URI);

	size_t pos = pathQuery.first.find("..");

	if (pos != std::string::npos && (pos == pathQuery.first.size() - 2 || pathQuery.first[pos + 2] == '/'))
		return (error(*this, 400, "Bad Request"));

	if (isCgi(pathQuery.first))
	{
		
	}

	return (getFile(pathQuery.first, *this));
}
