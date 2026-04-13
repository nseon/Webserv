/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ablock.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 15:56:20 by nseon             #+#    #+#             */
/*   Updated: 2026/04/13 18:01:42 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <map>

class Ablock {

typedef void (Ablock::*SetterFunc)(const std::string&);

	protected:
		std::string							_root;
		unsigned int						_client_max_body_size;
		std::string							_index;
		std::map<int, std::string>			_error_pages;
		std::map<std::string, SetterFunc>	_dispatchMap;
	public:
		virtual ~Ablock() = 0;
	
		void setRoot(std::string const &value);
		void setClientMaxBodySize(std::string const &value);
		void setIndex(std::string const &value);
		void addErrorPage(std::string const &value);
		
		void initDispatchMap();
};
