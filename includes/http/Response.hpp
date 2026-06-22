/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:07:59 by nseon             #+#    #+#             */
/*   Updated: 2026/06/18 13:41:19 by nseon            ###   ########.fr       */
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
		Response(void);

		int				_status_code;
		std::string		_status_msg;
		Location		*_location;
		Request const	*_request;

		int	handle_get(Request const& request);
		int	handle_post(Request const& request);
		int	handle_delete(Request const& request);

	public:
		Response(Request const &request, Location *location, int socketfd);
		Response(Location &location, std::string const& version);
		Response(int code, Server &server);
		~Response();

		void reset();

		int error(int code);
		void handle_redirection();
		bool buildFromCgiOutput(std::vector<char> const& raw);

		Location const& getLocation() const;
		Request const *getRequest() const;
		int getStatusCode() const;

		static std::string getStatusMessage(int code);
		static std::string getContentType(std::string const &path);

		void setStatusCode(int code);
		void setStatusMsg(std::string msg);
		void setBody(std::vector<char> body);
		void setBody(std::string const &str);

		std::string toString() const;
};
