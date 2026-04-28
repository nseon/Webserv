/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:39:51 by nseon             #+#    #+#             */
/*   Updated: 2026/04/20 14:18:16 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>
#include <iostream>

#include "Server.hpp"
#include "ListenerSocket.hpp"

Server::Server()
{
	_ip = DEFAULT_IP;
	_port = DEFAULT_PORT;
	_client_max_body_size = DEFAULT_MAX_SIZE;
	initDispatchMap();
	_socket = NULL;
	_dispatchMap["listen"] = reinterpret_cast<SetterFunc>(&Server::setPort);
	_dispatchMap["server_name"] = reinterpret_cast<SetterFunc>(&Server::setName);
	_dispatchMap["location"] = reinterpret_cast<SetterFunc>(&Server::addLocation);
}

Server::~Server() {}

//**********************SETTER**************************//

static std::string parseIp(std::string ip)
{
	std::stringstream ss(ip);
	int nb;
	char c;
	
	for (int i = 0; i < 4; ++i)
	{
		if (!(ss >> nb))
			throw std::logic_error("Invalid ip1: " + ip);
		if (!(0 <= nb && nb <= 255))
			throw std::logic_error("Invalid ip2: " + ip);
		if (i != 3)
		{
			if (ss >> c)
			{
				if (c != '.')
					throw std::logic_error("Invalid ip3: " + ip);
			}
			else
				throw std::logic_error("Invalid ip4: " + ip);
		}
	}
	return (ip);
}

void Server::setPort(std::string &value)
{
	size_t	i = value.find(':');
	
	if (i != std::string::npos)
	{
		_ip = parseIp(value.substr(0, i));
		value.erase(0, i + 1);
	}

	std::stringstream ss(value);
	unsigned int nb;
	std::string str;
	if (ss >> nb)
	{
		if (ss >> str)
			throw std::logic_error("Invalid port: " + value);
		_port = nb;
	}
	else
		throw std::logic_error("Invalid port: " + value);
}

void Server::setName(std::string const &value)
{
	std::stringstream ss(value);
	std::string name;
	
	ss >> name;
	if (ss >> name)
	{
		throw std::logic_error("Invalid name: " + value);
	}
	_name = value;
}

Location &Server::addLocation(std::string const &value)
{
	std::stringstream ss(value);
	std::string path;

	ss >> path;
	if (ss >> path)
		throw std::logic_error("Invalid location: " + value);
	Location new_loc(*this);
	new_loc.setPath(value);
	_locations.push_back(new_loc);
	return (_locations.back());
}

void	Server::createSocket(void)
{
	this->_socket = new ListenerSocket(this->getAddr(), this);
}

//**********************GETTER**************************//

unsigned int Server::getPort() const
{
	return (_port);
}

std::string Server::getIp() const
{
	return (_ip);
}

std::string	Server::getName() const
{
	return (_name);
}

std::vector<Location> Server::getLocations() const
{
	return (_locations);
}

struct sockaddr_in Server::getAddr() const
{
	struct sockaddr_in addr;

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_ip.c_str());

	return addr;
}

ListenerSocket*	Server::getSocket(void) const
{
	return (this->_socket);
}

//******************************************************//
		
Ablock &Server::createBlock(std::string token, std::string const &value)
{
	if (token != "location")
		throw std::logic_error("Can't create a block " + token + " in a server block");
	return (addLocation(value));
}

std::ostream &operator<<(std::ostream &os, const Server &serv) {
    os << "    server {\n";
    os << "        listen: " << serv.getIp() << ":" << serv.getPort() << "\n";
    os << "        server_name: " << serv.getName() << "\n";
    os << "        root: " << serv.getRoot() << "\n";
    os << "        index: " << serv.getIndex() << "\n";
    os << "        max_body_size: " << serv.getClientMaxBodySize() << "\n";

    // Error pages du serveur
    const std::map<int, std::string>& errors = serv.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
        os << "        error_page: " << it->first << " " << it->second << "\n";
    }

    // Itération sur les locations
    const std::vector<Location>& locs = serv.getLocations();
    for (size_t i = 0; i < locs.size(); ++i) {
        os << locs[i]; 
    }
    
    os << "    }\n";
    return os;
}
