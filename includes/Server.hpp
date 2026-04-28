/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:18:42 by nseon             #+#    #+#             */
/*   Updated: 2026/04/20 14:18:36 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#include "Ablock.hpp"
#include "ListenerSocket.hpp"
#include "Location.hpp"

#define DEFAULT_PORT 3030
#define DEFAULT_MAX_SIZE 10
#define DEFAULT_IP "0.0.0.0"

class Server : public Ablock{
	private:
		std::string _name;
		unsigned int _port;
		std::string _ip;
		
		std::vector<Location>	_locations;

		ListenerSocket*			_socket;

		void					createSocket(void);

	public:
		Server();
		~Server();

		void					setPort(std::string &value);
		void 					setName(std::string const &value);
		Location				&addLocation(std::string const &value);
		
		unsigned int			getPort() const;
		std::string				getIp() const;
		std::string				getName() const;
		std::vector<Location>	getLocations() const;
		struct sockaddr_in		getAddr() const;
		ListenerSocket*			getSocket() const;

		Ablock &createBlock(std::string token, std::string const &value);
};

std::ostream &operator<<(std::ostream &os, const Server &serv);
