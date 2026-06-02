/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 15:40:21 by nseon             #+#    #+#             */
/*   Updated: 2026/05/26 14:40:45 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include "http/Response.hpp"
#include "http/responses.hpp"

static std::vector<char> generate_default_error_page(int code, std::string msg)
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
	this->setStatusCode(code);
	this->setStatusMsg(msg);
	
	std::string error_path = this->getLocation().getErrorPath(code);
	
	if (error_path.empty())
	{
		std::stringstream ss;
		
		this->setBody(generate_default_error_page(code, msg));
		ss << this->getBody().size();
		this->addHeader("Content-Length", ss.str());
	}
	else
		if (getRessource(error_path, *this))
		{
			std::stringstream ss;
			
			this->setBody(generate_default_error_page(code, msg));
			ss << this->getBody().size();
			this->addHeader("Content-Length", ss.str());
		}
	return (this->getStatusCode());
}
