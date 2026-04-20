/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ablock.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 15:56:20 by nseon             #+#    #+#             */
/*   Updated: 2026/04/16 14:33:01 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>

class Ablock {	
	protected:
		typedef void (Ablock::*SetterFunc)(std::string const&);
	
		std::string							_root;
		unsigned int						_client_max_body_size;
		std::string							_index;
		std::map<int, std::string>			_error_pages;
		
		std::map<std::string, SetterFunc>	_dispatchMap;
	public:
		virtual ~Ablock();

		std::string							getRoot() const;
		unsigned int						getClientMaxBodySize() const;
		std::string							getIndex() const;
		std::map<int, std::string>			getErrorPages() const;
		std::map<std::string, SetterFunc>	getDispatchMap() const;

		void setRoot(std::string const &value);
		void setClientMaxBodySize(std::string const &value);
		void setIndex(std::string const &value);
		void addErrorPage(std::string const &value);

		void initDispatchMap();
		void launchAction(std::string &action, std::string &value);
		virtual Ablock &createBlock(std::string token, std::string const &value) = 0;
};
