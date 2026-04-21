/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 14:57:26 by nseon             #+#    #+#             */
/*   Updated: 2026/04/16 14:26:11 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

#include "Ablock.hpp"
#include "Server.hpp"

class Config : public Ablock{
	private:
		std::vector<Server> _servers;
	public:
		Config(std::string const &filename);
		~Config();
		
		std::vector<Server> getServers() const;

		Ablock &createBlock(std::string token, std::string const &value);
};

std::ostream &operator<<(std::ostream &os, const Config &conf);
