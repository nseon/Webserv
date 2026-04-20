/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 16:48:06 by nseon             #+#    #+#             */
/*   Updated: 2026/04/16 14:45:43 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Config.hpp"

int main(int argc, char **argv)
{
	if (argc == 2)
	{
		Config conf(argv[1]);
		std::cout << conf << std::endl;
	}
	else
		std::cout << "Usage: ./webserv file.conf" << std::endl;
}
