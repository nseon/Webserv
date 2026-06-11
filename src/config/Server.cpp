/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:39:51 by nseon             #+#    #+#             */
/*   Updated: 2026/05/25 13:39:02 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>
#include <iostream>
#include <vector>

#include "config/Server.hpp"
#include "config/Location.hpp"
#include "socket/ListenerSocket.hpp"

Server::Server()
{
	_ip = DEFAULT_IP;
	_port = DEFAULT_PORT;
	_client_max_body_size = DEFAULT_MAX_SIZE;
	initDispatchMap();
	_socket = NULL;
	_dispatchMap["listen"] = static_cast<SetterFunc>(&Server::setPort);
	_dispatchMap["server_name"] = static_cast<SetterFunc>(&Server::setName);
	_dispatchMap["location"] = static_cast<SetterFunc>(&Server::setLocation);
}

Server::~Server()
{
	if (this->_socket)
	{
		delete this->_socket;
	}
}

//**********************SETTER**************************//

static std::string parseIp(std::string const &ip)
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

void Server::setPort(std::string const &value)
{
	std::string	port(value);
	size_t		i = port.find(':');

	if (i != std::string::npos)
	{
		_ip = parseIp(port.substr(0, i));
		port.erase(0, i + 1);
	}

	std::stringstream ss(port);
	unsigned int nb;
	std::string str;
	if (ss >> nb)
	{
		if (ss >> str)
			throw std::logic_error("Invalid port: " + port);
		_port = nb;
	}
	else
		throw std::logic_error("Invalid port: " + port);
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

void Server::setLocation(std::string const &value)
{
	addLocation(value);
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
	this->_socket = new ListenerSocket(this->getSockAddr(), this);
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

struct sockaddr_in Server::getSockAddr() const
{
	struct sockaddr_in addr;

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = inet_addr(_ip.c_str());

	return addr;
}

std::string	Server::getAddr(void) const
{
	return (this->_socket->getAddress());
}

ListenerSocket*	Server::getSocket(void) const
{
	return (this->_socket);
}

Location *Server::matchLocation(std::string uri)
{
	Location *best_match = NULL;
	int longest = -1;
	
	for (std::vector<Location>::iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		int i = 0;
		while (it->getPath()[i])
		{
			if (!(uri[i]) || it->getPath()[i] != uri[i])
			{
				i = 0;
				break;
			}
			++i;
		}
		if (i > 0)
		{
			if (it->getPath()[i - 1] == '/' || uri[i] == '\0' || uri[i] == '/')
			{
				if (i > longest || best_match == NULL)
				{
					longest = i;
					best_match = &(*it);
				}
			}
		}
	}
	return (best_match);
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
