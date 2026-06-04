/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ablock.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 15:58:31 by nseon             #+#    #+#             */
/*   Updated: 2026/06/02 15:36:47 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <sstream>
#include <vector>
#include <iostream>

#include "config/Ablock.hpp"

typedef void (Ablock::*SetterFunc)(std::string const&);

Ablock::~Ablock() {}

//**********************GETTER**************************//

std::string	Ablock::getRoot() const
{
	return (_root);
}

unsigned int Ablock::getClientMaxBodySize() const
{
	return (_client_max_body_size);
}

std::string Ablock::getIndex() const
{
	return (_index);
}

std::map<int, std::string> Ablock::getErrorPages() const
{
	return (_error_pages);
}

std::string Ablock::getErrorPath(int code) const
{
	std::map<int, std::string>::const_iterator pos = _error_pages.find(code);
	
	if (pos != _error_pages.end())
		return (pos->second);
	return ("");
}

std::map<std::string, SetterFunc> Ablock::getDispatchMap() const
{
	return (_dispatchMap);
}

//**********************SETTER**************************//

void Ablock::setRoot(std::string const &value)
{
	std::stringstream ss(value);
	std::string path;
	
	ss >> path;
	if (ss >> path)
		throw std::logic_error("invalid index: " + value);
	if (path[path.size() - 1] == '/')
		path.erase(path.size() - 1, 1);
	_root = path;
}

void Ablock::setClientMaxBodySize(std::string const &value)
{
	std::stringstream ss(value);
	size_t nb;
	
	if (ss >> nb)
	{
		_client_max_body_size = nb;
		
		char unit;
		if (ss >> unit)
		{
			switch (unit)
			{
				case 'K': _client_max_body_size *= 1024; break;
				case 'M': _client_max_body_size *= 1024 * 1024; break;
				case 'G': _client_max_body_size *= 1024 * 1024 * 1024; break;
				default: throw std::logic_error("invalid unit for client max body size: " + value);
			}
		}
		std::string test;

		if (ss >> test)
			throw std::logic_error("invalid client max body size: " + value);
	}
	else
		throw std::logic_error("invalid client max body size: " + value);
}

void Ablock::setIndex(std::string const &value)
{
	std::stringstream ss(value);
	std::string file;
	
	ss >> file;
	if (ss >> file)
		throw std::logic_error("invalid index: " + value);
	_index = value;
}

void Ablock::addErrorPage(std::string const &value)
{
	std::stringstream ss(value);
	std::string path;
	std::vector<int> codes;
	int	code;

	while (ss >> code)
		codes.push_back(code);
	ss.clear();
	if (ss >> path)
	{
		if (ss >> path || !codes.size())
			throw std::logic_error("Invalid error page: " + value);
		for (std::vector<int>::iterator i = codes.begin(); i != codes.end(); ++i)
			_error_pages[*i] = path;
	}
	else
		throw std::logic_error("Invalid error page: " + value);
}

//**********************DISPATCH************************//

void Ablock::initDispatchMap()
{
	_dispatchMap["root"] = &Ablock::setRoot;
	_dispatchMap["client_max_body_size"] = &Ablock::setClientMaxBodySize;
	_dispatchMap["index"] = &Ablock::setIndex;
	_dispatchMap["error_page"] = &Ablock::addErrorPage;
}

void Ablock::launchAction(std::string &action, std::string &value)
{
	std::map<std::string, SetterFunc>::const_iterator it = _dispatchMap.find(action);

    if (it != _dispatchMap.end())
        (this->*(it->second))(value);
    else
        throw std::runtime_error("Unknown directive: " + action);
}
