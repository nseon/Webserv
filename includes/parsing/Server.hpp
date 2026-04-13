/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nseon <nseon@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:18:42 by nseon             #+#    #+#             */
/*   Updated: 2026/04/13 18:02:13 by nseon            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>

#include "Ablock.hpp"
#include "Location.hpp"

class Server : public Ablock{
	private:
		std::vector<Location> _locations;
	public:
		
};
