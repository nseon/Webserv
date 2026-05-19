/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 13:34:54 by nseon             #+#    #+#             */
/*   Updated: 2026/05/18 15:32:02 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Response.hpp"
#include "http/responses.hpp"
#include <fcntl.h>
#include <vector>

static void fill_response(Request const &request, Response &response)
{
	response.setVersion(request.getVersion());
	if (request.getMethod() == "GET")
		handle_get(request, response);
	if (request.getMethod() == "POST")
		handle_post(request, response);
	if (request.getMethod() == "DELETE")
		handle_delete(request, response);
}

//**********************GETTER**************************//

Location Response::getLocation() const
{
	return (_location);
}

int Response::getStatusCode() const
{
	return (_status_code);
}

//**********************SETTER**************************//

void Response::setStatusCode(int code)
{
	_status_code = code;
}

void Response::setStatusMsg(std::string msg)
{
	_status_msg = msg;
}

void Response::setBody(std::vector<char> body)
{
	_body = body;
}

//**************CONSTRUCTOR/DESTRUCTOR******************//

Response::Response(Request const &request, Location const &location) : _status_code(0), _location(location)
{
	fill_response(request, *this);
}

Response::~Response()
{}

