/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:11:20 by nseon             #+#    #+#             */
/*   Updated: 2026/06/01 12:58:40 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "http/Response.hpp"

int fill_error(Response &response, int code);
std::vector<char> generate_default_error_page(int code, std::string msg);
