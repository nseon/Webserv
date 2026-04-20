/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:19:42 by nseon             #+#    #+#             */
/*   Updated: 2026/04/16 15:35:56 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Ablock.hpp"

class Location : public Ablock{
	private:
		std::string 						_path;
		bool								_allow_get;
		bool								_allow_post;
		bool								_allow_delete;
		bool								_autoindex;
		std::string							_upload_store;
		std::pair<int, std::string>			_return;
		std::map<std::string, std::string>	_cgi_configs;
	public:
		Location(Ablock const &m);
		~Location();

		void setPath(std::string const &value);
		void setMethods(std::string const &value);
		void setAutoIndex(std::string const &value);
		void setUploadStore(std::string const &value);
		void setReturn(std::string const &value);
		void addCgi(std::string const &value);
		
		std::string getPath() const;
		bool getAllowGet() const;
		bool getAllowPost() const;
		bool getAllowDelete() const;
		bool getAutoIndex() const;
		std::string getUploadStore() const;
		std::pair<int, std::string> getReturn() const;
		std::map<std::string, std::string> getCgiConfigs() const;
		
		Ablock &createBlock(std::string token, std::string const &value);
};

std::ostream &operator<<(std::ostream &os, const Location &loc);
