/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:01:26 by nseon             #+#    #+#             */
/*   Updated: 2026/06/01 15:26:19 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
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
#include <dirent.h>

int getFile(std::string const &path, Response &response);

std::string generate_autoindex(const std::string &path, const std::string &uri)
{
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) {
        return "";
    }

    std::ostringstream html;
    
    html << "<html>\n<head><title>Index of " << uri << "</title></head>\n";
    html << "<body>\n<h1>Index of " << uri << "</h1>\n<hr>\n<pre>\n";

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        if (name == ".") {
            continue;
        }

        if (name == ".." && uri == "/") {
            continue;
        }

        std::string full_path = path;
        if (full_path[full_path.length() - 1] != '/')
            full_path += "/";
        full_path += name;

        struct stat st;
        if (stat(full_path.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            name += "/";
        }

        html << "<a href=\"" << name << "\">" << name << "</a>\n";
    }

    closedir(dir);

    html << "</pre>\n<hr>\n</body>\n</html>\n";

    return html.str();
}

int handle_directory(std::string const &path, Response &response)
{
	if (!response.getRequest()->getServer()->getIndex().empty())
	{
		std::string path = response.getLocation()->getRoot() + "/" + response.getRequest()->getServer()->getIndex();
		std::cout << path << std::endl;
		getFile(path, response);
		response.setStatusCode(200);
		response.setStatusMsg("OK");
	}
	else if (response.getLocation()->getAutoIndex())
	{
		std::string autoindex = generate_autoindex(path, response.getRequest()->getPath());
		std::stringstream ss;
		
		response.setBody(autoindex);
		ss << response.getBody().size();
		response.addHeader("Content-Length", ss.str());
		response.setStatusCode(200);
		response.setStatusMsg("OK");
	}
	else
		return (403);
	return (200);
}

int getRessource(std::string const &path, Response &response)
{
	struct stat st;

	if (stat(path.c_str(), &st) == -1)
		return (404);
	else if (S_ISDIR(st.st_mode))
		return (handle_directory(path, response));
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
		return (200);
	}
}

int getFile(std::string const &path, Response &response)
{
	switch (getRessource(path, response))
	{
		case 404: return (fill_error(response, 404));
		case 403: return (fill_error(response, 403));
		case 500: return (fill_error(response, 500));
	}
	return (0);
}

int handle_get(Request const &request, Response &response)
{
	if (!response.getLocation()->getAllowGet())
		return (fill_error(response, 405));
	response.setStatusCode(200);
	response.setStatusMsg("OK");

	std::string	path = response.getLocation()->getRoot() + request.getPath();
	size_t		pos = path.find("..");

	if (pos != std::string::npos && (pos == path.size() - 2 || path[pos + 2] == '/'))
		return (fill_error(response, 400));

	return (getFile(path, response));
}
