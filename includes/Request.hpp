/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 14:02:10 by nseon             #+#    #+#             */
/*   Updated: 2026/05/06 11:26:51 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstddef>
#include <string>
#include <map>
#include <vector>

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

class Request {
	private:
		std::string _raw_data;
		int	_parsing_state;
		int	_chunk_state;
		size_t	_chunk_size;

		std::string _method;
		std::string _path;
		std::string _version;
		std::map<std::string, std::string>	_headers;
		std::vector<char> _body;
		
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
		void setVersion(std::string const &value);
		void addHeader(std::string const &key, std::string const &value);
		
		int getParsingState() const;
		std::string getRawData() const;
		std::string getMethod() const;
		std::string getPath() const;
		std::string getVersion() const;
		std::map<std::string, std::string> getHeaders() const;
		std::vector<char> getBody() const;
		
		void reset();
};

std::ostream &operator<<(std::ostream &os, const Request &req);
