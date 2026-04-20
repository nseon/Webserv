/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:39:59 by nseon             #+#    #+#             */
/*   Updated: 2026/04/17 13:43:44 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "Location.hpp"

Location::Location(Ablock const &m)
{
	_root = m.getRoot();
	_client_max_body_size = m.getClientMaxBodySize();
	_index = m.getIndex();
	_error_pages = m.getErrorPages();
	_autoindex = false;
	_allow_delete = false;
	_allow_get = false;
	_allow_post = false;
	initDispatchMap();
	_dispatchMap["limit_except"] = reinterpret_cast<SetterFunc>(&Location::setMethods);
	_dispatchMap["autoindex"] = reinterpret_cast<SetterFunc>(&Location::setAutoIndex);
	_dispatchMap["upload_store"] = reinterpret_cast<SetterFunc>(&Location::setUploadStore);
	_dispatchMap["return"] = reinterpret_cast<SetterFunc>(&Location::setReturn);
	_dispatchMap["cgi"] = reinterpret_cast<SetterFunc>(&Location::addCgi);
}

Location::~Location()
{}

//**********************SETTER**************************//

void Location::setPath(std::string const &value)
{
	std::stringstream ss(value);
	std::string path;

	ss >> path;
	if (ss >> path)
		throw std::logic_error("Invalid path: " + value);
	_path = value;
}

void Location::setMethods(std::string const &value)
{
	std::stringstream ss(value);
	std::string str;
	
	while (ss >> str)
	{
		if (str == "GET")
			_allow_get = true;
		else if (str == "POST")
			_allow_post = true;
		else if (str == "DELETE")
			_allow_delete = true;
		else
			throw std::logic_error("Unknow method: " + str);
	}
}

void Location::setAutoIndex(std::string const &value)
{
	std::stringstream ss(value);
	std::string state;

	ss >> state;
	if (ss >> state)
		throw std::logic_error("Invalid state for autoindex: " + value);
	if (state == "on")
		_autoindex = true;
}

void Location::setUploadStore(std::string const &value)
{
	std::stringstream ss(value);
	std::string path;
	
	ss >> path;
	if (ss >> path)
		throw std::logic_error("Invalid path for upload_store: " + value);
	_upload_store = value;
}

void Location::setReturn(std::string const &value)
{
	std::stringstream ss(value);
	std::string str;
	int nb = -1;

	ss >> nb;
	ss.clear();
	if (ss >> str)
	{
		if (ss >> str)
			throw std::logic_error("Invalid return: " + value);
		_return.first = nb;
		_return.second = str; 
	}
	else
		throw std::logic_error("Invalid return: " + ss.str());
}

void Location::addCgi(std::string const &value)
{
	std::stringstream ss(value);
	std::string	extension;
	std::string path;

	if (ss >> extension >> path)
	{
		if (ss >> path)
			throw std::logic_error("Invalid return: " + value);
		_cgi_configs[extension] = path;
	}
	else
		throw std::logic_error("Invalid cgi: " + value);		
}

//**********************GETTER**************************//

std::string Location::getPath() const
{
	return (_path);
}

bool Location::getAllowGet() const
{
	return (_allow_get);
}

bool Location::getAllowPost() const
{
	return (_allow_post);
}

bool Location::getAllowDelete() const
{
	return (_allow_delete);
}

bool Location::getAutoIndex() const
{
	return (_autoindex);
}

std::string Location::getUploadStore() const
{
	return (_upload_store);
}

std::pair<int, std::string> Location::getReturn() const
{
	return (_return);
}

std::map<std::string, std::string> Location::getCgiConfigs() const
{
	return(_cgi_configs);
}

//******************************************************//


Ablock &Location::createBlock(std::string token, std::string const &)
{
	throw std::logic_error("Can't create a block " + token + " in a location block");
}

std::ostream &operator<<(std::ostream &os, const Location &loc) {
    os << "        location " << (loc.getPath().empty() ? "[no path]" : loc.getPath()) << " {\n";
    
    // Méthodes autorisées
    os << "            methods: [";
    if (loc.getAllowGet()) os << " GET";
    if (loc.getAllowPost()) os << " POST";
    if (loc.getAllowDelete()) os << " DELETE";
    os << " ]\n";

    os << "            root: " << loc.getRoot() << "\n";
    os << "            index: " << loc.getIndex() << "\n";
    os << "            autoindex: " << (loc.getAutoIndex() ? "on" : "off") << "\n";
    os << "            upload_store: " << loc.getUploadStore() << "\n";
    os << "            return: " << loc.getReturn().first << " " << loc.getReturn().second << "\n";

    // Affichage des CGI
    const std::map<std::string, std::string>& cgis = loc.getCgiConfigs();
    for (std::map<std::string, std::string>::const_iterator it = cgis.begin(); it != cgis.end(); ++it) {
        os << "            cgi: " << it->first << " -> " << it->second << "\n";
    }

    // Error pages
    const std::map<int, std::string>& errors = loc.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
        os << "            error_page: " << it->first << " " << it->second << "\n";
    }

    os << "            max_body_size: " << loc.getClientMaxBodySize() << "\n";
    os << "        }\n";
    return os;
}
