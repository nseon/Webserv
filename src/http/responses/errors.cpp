/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 15:40:21 by nseon             #+#    #+#             */
/*   Updated: 2026/06/17 16:02:38 by nseon            ###   ########.fr       */
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

int Response::error(int code)
{
	std::string msg = Response::getStatusMessage(code);

	std::string error_path = this->getRequest()->getServer()->getRoot();
	Location const* loc = this->_location;

	if (!loc)
	{
		if (this->getRequest()->getServer()->getErrorPath(code)[0] != '/')
			error_path += '/';
		error_path += this->getRequest()->getServer()->getErrorPath(code);
	}
	else
	{
		if (loc->getErrorPath(code)[0] != '/')
			error_path += '/';
		error_path += loc->getErrorPath(code);
	}
	if (error_path == this->getRequest()->getServer()->getRoot() + '/')
	{
		std::stringstream ss;

		this->setBody(generate_default_error_page(code, msg));
		ss << this->getBody().size();
		this->addHeader("Content-Length", ss.str());
	}
	else if (getRessource(error_path, *this) != 200)
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
