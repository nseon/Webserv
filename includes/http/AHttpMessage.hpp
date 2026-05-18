/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AHttpMessage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/06 16:26:13 by nseon             #+#    #+#             */
/*   Updated: 2026/05/11 13:44:05 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <vector>

class AHttpMessage {
	protected:
		std::string _version;
		std::map<std::string, std::string>	_headers;
		std::vector<char> _body;
	public:
		virtual ~AHttpMessage();

		void setVersion(std::string const &value);
		void addHeader(std::string const &key, std::string const &value);

		std::string getVersion() const;
		std::map<std::string, std::string> getHeaders() const;
		std::vector<char> getBody() const;

		virtual void reset() = 0;
};
