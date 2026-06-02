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
#include "http/Response.hpp"
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
		case 404: return (response.error(404, "Not found"));
		case 403: return (response.error(403, "Forbidden"));
		case 500: return (response.error(500, "Internal Server Error"));
	}
	return (0);
}

int Response::handle_get(Request const &request)
{
	if (!this->getLocation().getAllowGet())
		return (this->error(405, "Method not allowed"));
	this->setStatusCode(200);
	this->setStatusMsg("OK");

	std::string	path = request.getPath();
	size_t		query = path.find('?');

	if (query != std::string::npos)
		path = path.substr(0, query);

	size_t	pos = path.find("..");

	if (pos != std::string::npos && (pos == path.size() - 2 || path[pos + 2] == '/'))
		return (this->error(400, "Bad Request"));

	std::string	full = this->getLocation().getRoot() + path;
	return (getFile(full, *this));
}
