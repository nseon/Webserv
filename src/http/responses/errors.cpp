/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 15:40:21 by nseon             #+#    #+#             */
/*   Updated: 2026/06/11 14:16:32 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include "config/Location.hpp"
#include "http/Response.hpp"
#include "http/responses.hpp"

std::vector<char> generate_default_error_page(int code, std::string msg)
{
	std::stringstream ss;
	std::vector<char> body;

	ss << code;

	std::string html("<html><body><h1>Error " + ss.str() + " " + msg + "</h1></body></html>");

	body.insert(body.end(), html.begin(), html.end());
	return (body);
}

int Response::error(int code, std::string msg)
{
	std::string error_path;

	if (_location)
		error_path = _location->getErrorPath(code);
	else if (_request)
		error_path = _request->getServer()->getErrorPath(code);

	if (error_path.empty() || getRessource(error_path, *this) != 200)
	{
		std::stringstream ss;

		this->setBody(generate_default_error_page(code, msg));
		ss << this->getBody().size();
		this->addHeader("Content-Length", ss.str());
	}
	this->setStatusCode(code);
	this->setStatusMsg(msg);
	return (this->getStatusCode());
}
