/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:05:52 by nseon             #+#    #+#             */
/*   Updated: 2026/06/04 15:40:13 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/stat.h>

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/errors.hpp"

static int delete_ressource(std::string &path, Response &response)
{
	struct stat sb;
	
	if (stat(path.c_str(), &sb) != 0)
		return (fill_error(response, 404));
	if (S_ISDIR(sb.st_mode))
		return (fill_error(response, 403));
	if (unlink(path.c_str()) != 0)
		return (fill_error(response, 500));
	response.setStatusCode(204);
	response.setStatusMsg(Response::getStatusMessage(204));
	response.addHeader("Content-Length", "0");
	return (204);
}

int handle_delete(Request const &request, Response &response)
{
	if (!response.getLocation()->getAllowDelete())
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
	return (delete_ressource(path, response));
}
