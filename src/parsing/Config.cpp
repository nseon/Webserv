/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 14:59:37 by nseon             #+#    #+#             */
/*   Updated: 2026/04/17 11:37:36 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <sstream>
#include <iostream>

#include "Config.hpp"

static std::string extractSubContent(std::stringstream &ss)
{
	std::string token;
	std::string content;
	int count = 0;

	ss >> token;
	if (token != "{")
		throw std::logic_error("'{' was expected, not " + token);
	else
		count++;
	ss >> token;
	while (count)
	{
		content += token + " ";
		ss >> token;
		// std::cout << token << std::endl;
		if (token == "{")
			count++;
		else if (token == "}")
			count--;
	}
	return (content);
}

static void parseBloc(Ablock &block, std::string const &content)
{
	std::stringstream ss(content);
	std::string token;

	while (ss >> token)
	{
		if (token == "server" || token == "location")
		{
			std::string value;
			if (token == "location")
				ss >> value;
			parseBloc(block.createBlock(token, value), extractSubContent(ss));
		}
		else
		{
			std::string value;
			std::string action = token;
			while (token[token.size() - 1] != ';')
			{
				if (!(ss >> token))
					break ;
				value += token + " ";
			}
			value.erase(value.find(';'));
			block.launchAction(action, value);
		}
	}
}

Config::Config(std::string const &filename)
{
	_client_max_body_size = 10;
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Failed to open: " + filename);
	
	std::string content;
	std::string line;
	
	while (std::getline(file, line))
	{
		size_t commentPos = line.find('#');
        if (commentPos != std::string::npos)
            line = line.substr(0, commentPos);
		content += line + " ";
	}
	file.close();
	try {
		parseBloc(*this, content);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

Config::~Config()
{}

std::vector<Server> Config::getServers() const
{
	return (_servers);
}


Ablock &Config::createBlock(std::string token, std::string const &)
{
	if (token == "server")
	{
		Server new_serv;

		_servers.push_back(new_serv);
	}
	else
		throw std::logic_error("Can't create a block " + token + " out a server block");
	return (_servers.back());
}

std::ostream &operator<<(std::ostream &os, const Config &conf) {
    os << "\n" << std::string(50, '=') << "\n";
    os << "         WEBSERV CONFIGURATION SUMMARY\n";
    os << std::string(50, '=') << "\n";

    const std::vector<Server>& servers = conf.getServers();

    if (servers.empty()) {
        os << " [!] No servers found in configuration.\n";
    } else {
        for (size_t i = 0; i < servers.size(); ++i) {
            os << "\n [Server #" << i + 1 << "]\n";
            os << servers[i]; // Utilise l'operator<< de Server
        }
    }

    os << std::string(50, '=') << "\n";
    return os;
}
