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

int fill_error(Response &response, int code)
{
	std::string msg = Response::getStatusMessage(code);

	std::string error_path = response.getRequest()->getServer()->getRoot();
	Location *loc = response.getLocation();
	
	if (!loc)
	{
		if (response.getRequest()->getServer()->getErrorPath(code)[0] != '/')
			error_path += '/';
		error_path += response.getRequest()->getServer()->getErrorPath(code);
	}
	else
	{
		if (loc->getErrorPath(code)[0] != '/')
			error_path += '/';
		error_path += loc->getErrorPath(code);
	}
	if (error_path == response.getRequest()->getServer()->getRoot() + '/')
	{
		std::stringstream ss;
		
		response.setBody(generate_default_error_page(code, msg));
		ss << response.getBody().size();
		response.addHeader("Content-Length", ss.str());
	}
	else if (getRessource(error_path, response) != 200)
	{
		std::stringstream ss;
		
		response.setBody(generate_default_error_page(code, msg));
		ss << response.getBody().size();
		response.addHeader("Content-Length", ss.str());
	}
	response.setStatusCode(code);
	response.setStatusMsg(msg);
	return (response.getStatusCode());
}
