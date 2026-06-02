/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   post.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/11 14:05:47 by nseon             #+#    #+#             */
/*   Updated: 2026/05/19 11:43:04 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "http/Response.hpp"

int Response::handle_post(Request const &request)
{
	(void)request;
	return (this->error(501, "Not Implemented"));
}
