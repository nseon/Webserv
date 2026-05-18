/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/07 16:07:59 by nseon             #+#    #+#             */
/*   Updated: 2026/05/13 14:10:22 by nseon            ###   ########.fr       */
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
		Location _location;
	public:
		Response(Request const &request, Location const &location);
		~Response();

		Location getLocation() const;
		int getStatusCode() const;

		void setStatusCode(int code);
		void setStatusMsg(std::string msg);
		void setBody(std::vector<char> body);

		void toString();
};
