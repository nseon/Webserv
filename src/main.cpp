/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 16:48:06 by nseon             #+#    #+#             */
/*   Updated: 2026/06/09 13:45:41 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

#include "config/Config.hpp"
#include "manager/ServerManager.hpp"

int	main(int argc, char **argv)
{
	if (argc == 2)
	{
		try {
			Config			conf(argv[1]);
			
			for (size_t i = 0; i < conf.getServers().size(); ++i)
			{
				if (conf.getServers()[i].getLocations().empty())
				{
					std::cout << "Need at least a Location in a Server" << std::endl;
					return (1);
				}
			}

			ServerManager	sm(conf.getServers());
			
	
			std::cout << conf << std::endl;
			sm.serverLoop();
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	else
		std::cout << "Usage: ./webserv file.conf" << std::endl;
}
