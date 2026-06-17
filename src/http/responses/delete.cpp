/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:05:52 by nseon             #+#    #+#             */
/*   Updated: 2026/06/11 14:16:32 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/stat.h>

#include "http/Request.hpp"
#include "http/Response.hpp"

static int delete_ressource(std::string &path, Response &response)
{
	struct stat sb;

	if (stat(path.c_str(), &sb) != 0)
		return (response.error(404));
	if (S_ISDIR(sb.st_mode))
		return (response.error(403));
	if (unlink(path.c_str()) != 0)
		return (response.error(500));
	response.setStatusCode(204);
	response.setStatusMsg(Response::getStatusMessage(204));
	response.addHeader("Content-Length", "0");
	return (204);
}

int Response::handle_delete(Request const &request)
{
	if (!this->getLocation().getAllowDelete())
		return (this->error(405));
	if (this->getLocation().getUploadStore().empty())
		return (this->error(403));

	size_t pos = request.getPath().rfind("/");
	std::string filename;

	if (pos != std::string::npos)
		filename = request.getPath().substr(pos + 1);
	else
		filename = request.getPath();
	if (filename.empty())
		return (this->error(400));

	std::string path = this->getLocation().getUploadStore() + "/" + filename;
	return (delete_ressource(path, *this));
}
