/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:07:59 by nseon             #+#    #+#             */
/*   Updated: 2026/06/01 14:27:47 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "http/AHttpMessage.hpp"
#include "http/Request.hpp"
#include "config/Location.hpp"

class Response : public AHttpMessage {
	private:
		int	_status_code;
		std::string _status_msg;
		Location *_location;
		Request *_request;
	public:
		Response(int error_code, Server &server);
		Response(Request &request, Location &location);
		~Response();

		int getStatusCode() const;
		Location *getLocation() const;
		Request *getRequest() const;

		void setStatusCode(int code);
		void setStatusMsg(std::string msg);
		void setBody(std::vector<char> body);
		void setBody(std::string const &str);

		void handle_redirection();

		std::string toString() const;

		static std::string getStatusMessage(int code);
};
