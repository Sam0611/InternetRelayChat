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

bool is_only_numbers(char *port)
{
	for (int i = 0; port[i]; i++)
		if (!isdigit(port[i]))
			return (false);
	return (true);
}

int	main(int ac, char **av)
{
	if (ac != 3)
	{
        std::cerr << RED << "Error: Wrong number of arguments" << RESET << std::endl;
		return (1);
	}

	std::string password(av[2]);

	Server server(password);

	if (!is_only_numbers(av[1]))
	{
        std::cerr << RED << "Error: Invalid port number" << RESET << std::endl;
		return (1);
	}

    if (password.empty())
    {
        std::cerr << RED << "Error: Password can't be empty" << RESET << std::endl;
        return (1);
    }

	if (server.createServer(av[1]) == ERROR)
		return (1);

	if (server.startServer() == ERROR)
		return(1);


	return (0);
}
