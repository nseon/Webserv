/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:07:59 by nseon             #+#    #+#             */
/*   Updated: 2026/05/25 14:57:54 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "http/AHttpMessage.hpp"
#include "http/Request.hpp"
#include "config/Location.hpp"
#include "socket/ClientSocket.hpp"

# ifndef CGIMODE_GET
#  define CGIMODE_GET
# endif

# ifndef CGIMODE_POST
#  define CGIMODE_POST
# endif

class Response : public AHttpMessage {
	private:
		Response(void);

		int				_status_code;
		std::string		_status_msg;
		std::string		_cgi;
		Location		*_location;
		ClientSocket&	_client;

		int error(int code, std::string msg);

		int	handle_get(Request const& request);
		int	handle_post(Request const& request);
		int	handle_delete(Request const& request);

		int									handle_cgi(std::pair<std::string, std::string> const& pathQuery, int mode);
		char**								createCgiEnvs(std::pair<std::string, std::string> const& pathQuery);
		bool								isCgi(std::string const& URI);
		bool								isExec(std::string const& path);
		std::pair<std::string, std::string>	parsePathQuery(std::string const& URI);

	public:
		Response(Request const &request, Location &location);
		~Response();

		Location getLocation() const;
		int getStatusCode() const;

		void setStatusCode(int code);
		void setStatusMsg(std::string msg);
		void setBody(std::vector<char> body);

		std::string toString() const;
};
