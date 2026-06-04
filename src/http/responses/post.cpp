/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:05:47 by nseon             #+#    #+#             */
/*   Updated: 2026/06/04 14:08:53 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/errors.hpp"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <map>
#include <vector>

static std::vector<char> getContent(Request const &request)
{
	std::map<std::string, std::string>::const_iterator it = request.getHeaders().find("content-type");
	
	if (it == request.getHeaders().end())
		return request.getBody();

	size_t pos = it->second.find("boundary=");

	if (pos == std::string::npos)
		return (request.getBody());

	std::string boundary = it->second.substr(pos + 9);
	std::string target = "\r\n\r\n";
	std::vector<char>::const_iterator begining = std::search(request.getBody().begin(), request.getBody().end(), target.begin(), target.end()) + 4;
	std::vector<char>::const_iterator ending = std::search(begining, request.getBody().end(), boundary.begin(), boundary.end()) - 4;
	std::vector<char> body(begining, ending);

	return (body);
}

int post_ressource(std::string &path, Request const &request, Response &response)
{
	std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);
	
	if (!file)
		return fill_error(response, 500);
	
	std::vector<char> const &body = getContent(request);

	if (!body.empty())
		file.write(&body[0], body.size());
	file.close();
	response.setStatusCode(201);
	response.setStatusMsg(Response::getStatusMessage(201));
	response.addHeader("Content-Length", "0");
	return (201);
}

int handle_post(Request const &request, Response &response)
{
	if (!response.getLocation()->getAllowPost())
		return (fill_error(response, 405));
	if (response.getLocation()->getUploadStore().empty())
		return (fill_error(response, 403));
	
	size_t pos = request.getPath().rfind("/");
	std::string filename;

	if (pos != std::string::npos)
		filename = request.getPath().substr( pos + 1);
	else
		filename = request.getPath();
	if (filename.empty())
		return (fill_error(response, 400));
	
	std::string path = response.getLocation()->getUploadStore() + "/" + filename;
	
	return (post_ressource(path, request, response));
}
