/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 14:02:10 by nseon             #+#    #+#             */
/*   Updated: 2026/05/26 17:31:56 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <string>
#include <map>

#include "config/Server.hpp"
#include "http/AHttpMessage.hpp"

enum parsing_state {
	REQUEST,
	HEADERS,
	BODY,
	TRAILERS,
	DONE,
};

enum chunk_state {
	CHUNK_SIZE,
	CHUNK_DATA,
	CHUNK_CRLF,
};

class Request : public AHttpMessage {
	private:
		std::string _raw_data;
		int	_parsing_state;
		int	_chunk_state;
		size_t	_chunk_size;

		std::string _method;
		std::string _path;
		
		Server *_server;

		bool _handleRequestLine();
		bool _handleHeaders();
		bool _handleBody();
		bool _parseBodyContent(std::map<std::string, std::string>::iterator &i);
		bool _parseBodyChunked();
	public:
		Request();
		~Request();

		bool parseRequest(std::string msg);

		void setMethod(std::string const &value);
		void setPath(std::string const &value);
		void setServer(Server *ptr);
		
		int getParsingState() const;
		std::string getRawData() const;
		std::string getMethod() const;
		std::string getPath() const;
		Server *getServer() const;

		void reset();
};

std::ostream &operator<<(std::ostream &os, const Request &req);
