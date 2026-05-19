/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   responses.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:04:38 by nseon             #+#    #+#             */
/*   Updated: 2026/05/19 11:43:12 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "http/Request.hpp"
#include "http/Response.hpp"

int handle_get(Request const &request, Response &response);
int handle_post(Request const &request, Response &response);
int handle_delete(Request const &request, Response &response);
