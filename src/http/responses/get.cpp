/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:01:26 by nseon             #+#    #+#             */
/*   Updated: 2026/05/19 11:26:00 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/errors.hpp"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

void handle_directory()
{}

int getRessource(std::string const &path, Response &response)
{
	struct stat st;

	if (stat(path.c_str(), &st))
		return (error(response, 404, "Not found"));
	else if (S_ISDIR(st.st_mode))
	{
		handle_directory();
		return (0);
	}
	else if (!S_ISREG(st.st_mode) || !(st.st_mode & S_IRUSR))
		return (error(response, 403, "Forbidden"));
	else
	{
		std::stringstream ss;

		ss << st.st_size;
		response.addHeader("Content-Length", ss.str());

		std::ifstream file(path.c_str(), std::ios::binary);

		if (!file)
			return (error(response, 500, "Internal Server Error"));

		std::vector<char> content = std::vector<char>((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

		response.setBody(content);
		response.setStatusCode(200);
	}
	return (0);
}

int handle_get(Request const &request, Response &response)
{
	if (!response.getLocation().getAllowGet())
		return (error(response, 405, "Method not allowed"));
	else
	{
		response.setStatusCode(200);
		response.setStatusMsg("OK");
		
		std::string path = response.getLocation().getRoot() + request.getPath();

		size_t pos = path.find("..");
		
		if (pos != std::string::npos && (pos == path.size() - 2 || path[pos + 2] == '/'))
			return (error(response, 400, "Bad Request"));

		getRessource(path, response);
	}
}
