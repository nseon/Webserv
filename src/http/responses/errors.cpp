/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 15:40:21 by nseon             #+#    #+#             */
/*   Updated: 2026/05/13 14:11:00 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>
#include <vector>
#include "http/Response.hpp"

static std::vector<char> generate_default_error_page(int code, std::string msg)
{
	std::stringstream ss;
	std::vector<char> body;
	
	ss << code;
	
	std::string html("<html><body><h1>Error " + ss.str() + " " + msg + "</h1></body></html>");
	
	body.insert(body.end(), html.begin(), html.end());
	return (body);
}

int error(Response &response, int code, std::string msg)
{
	response.setStatusCode(code);
	response.setStatusMsg(msg);
	
	std::string error_path = response.getLocation().getErrorPath(code);
	
	if (error_path.empty())
		response.setBody(generate_default_error_page(code, msg));
	else
	 	;
	return (response.getStatusCode());
}
