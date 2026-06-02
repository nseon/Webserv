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

class Response : public AHttpMessage {
	private:
		Response(void);

		int				_status_code;
		std::string		_status_msg;
		Location		*_location;

		int	handle_get(Request const& request);
		int	handle_post(Request const& request);
		int	handle_delete(Request const& request);

	public:
		Response(Request const &request, Location &location);
		Response(Location &location, std::string const& version);
		~Response();

		void reset();

		int error(int code, std::string msg);
		bool buildFromCgiOutput(std::vector<char> const& raw);

		Location getLocation() const;
		int getStatusCode() const;

		void setStatusCode(int code);
		void setStatusMsg(std::string msg);
		void setBody(std::vector<char> body);

		std::string toString() const;
};
