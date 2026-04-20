/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 16:48:06 by nseon             #+#    #+#             */
/*   Updated: 2026/04/20 14:33:01 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <netinet/in.h>
#include <cstring>

#include "Config.hpp"
#include "PollingManager.hpp"

int	main(int argc, char **argv)
{
	if (argc == 2)
	{
		Config conf(argv[1]);
		
		std::cout << conf << std::endl;
		
		struct sockaddr_in		address = conf.getServers()[0].getAddr();
		PollingManager			pm;
		std::vector<ASocket*>	readyList;

		pm.addListenerSocket(address);
		while (1)
		{
			readyList = pm.poll();
			for (unsigned int i = 0; i < readyList.size(); i++)
			{
				readyList[i]->socketBehavior(&pm);
			}
		}
	}
	else
		std::cout << "Usage: ./webserv file.conf" << std::endl;
}
