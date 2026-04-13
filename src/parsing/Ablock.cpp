/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ablock.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 15:58:31 by nseon             #+#    #+#             */
/*   Updated: 2026/04/13 17:59:40 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>

#include "Ablock.hpp"

void Ablock::setRoot(std::string const &value)
{
	_root = value;
}

void Ablock::setClientMaxBodySize(std::string const &value)
{
	std::stringstream ss(value);
	int nb;
	
	ss >> nb;
	_client_max_body_size = nb;
}

void Ablock::setIndex(std::string const &value)
{
	_index = value;	
}

void Ablock::addErrorPage(std::string const &value)
{
	std::stringstream ss(value);
	int code;
	std::string path;

	if (ss >> code >> path)
		_error_pages[code] = path;
	else
		throw std::logic_error("Invalid line: " + value);
}

void Ablock::initDispatchMap()
{
	_dispatchMap["root"] = &Ablock::setRoot;
	_dispatchMap["client_max_body_size"] = &Ablock::setClientMaxBodySize;
	_dispatchMap["index"] = &Ablock::setIndex;
	_dispatchMap["error_page"] = &Ablock::addErrorPage;
}
