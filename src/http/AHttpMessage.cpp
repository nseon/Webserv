/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMessage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 11:37:15 by nseon             #+#    #+#             */
/*   Updated: 2026/06/04 12:25:22 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/AHttpMessage.hpp"

AHttpMessage::~AHttpMessage()
{}

//**********************GETTER**************************//

std::string AHttpMessage::getVersion() const
{
	return (_version);
}

std::map<std::string, std::string> const &AHttpMessage::getHeaders() const
{
	return (_headers);
}

std::vector<char> const &AHttpMessage::getBody() const
{
	return (_body);
}

//**********************SETTER**************************//

void AHttpMessage::setVersion(std::string const &value)
{
	if (value != "HTTP/1.0" && value != "HTTP/1.1")
		throw 505;
	_version = value;
}

void AHttpMessage::addHeader(std::string const &key, std::string const &value)
{
	_headers[key] = value;
}
