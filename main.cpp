/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 17:59:28 by sam               #+#    #+#             */
/*   Updated: 2024/06/07 18:20:03 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

int	main(int ac, char **av)
{
	if (ac != 2) // 3 mdp
	{
        std::cerr << RED << "Error: Wrong number of arguments" << RESET << std::endl;
		return (1);
	}


	Server server;

	// check av[1] is number (and range is valid) [to do]

	if (server.createServer(av[1]) == ERROR)
		return (1);

	if (server.startServer() == ERROR)
		return(1);


	return (0);
}
