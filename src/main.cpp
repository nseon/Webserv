/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 16:48:06 by nseon             #+#    #+#             */
/*   Updated: 2026/04/20 14:48:42 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <netinet/in.h>
#include <cstring>

#include "Config.hpp"
#include "Logger.hpp"
#include "PollingManager.hpp"
#include "ServerManager.hpp"

int	main(int argc, char **argv)
{
	if (argc == 2)
	{
		Config			conf(argv[1]);
		ServerManager	sm(conf.getServers());
		

		std::cout << conf << std::endl;
		sm.serverLoop();
	}
	else
		std::cout << "Usage: ./webserv file.conf" << std::endl;
}
