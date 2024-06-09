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

#include <poll.h> // poll

#include "Server.hpp"

int	main(int ac, char **av)
{
	if (ac != 2) // 3 mdp
	{
        std::cerr << RED << "Error: Wrong number of arguments" << RESET << std::endl;
		return (1);
	}


	Server server;

	// check av[1] is number (and range is valid)

	int sockfd = server.createServer(av[1]);
	if (sockfd == ERROR)
		return (1);


	struct pollfd fds[10];
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;
	int nfds = 1;
	int newClient = -1;
	sockaddr_in addrClient;
	socklen_t len = sizeof(addrClient);
	char buffer[1024];
	for (size_t i = 0; i < sizeof(buffer); i++)
		buffer[i] = 0;
	while (1)
	{
		if (poll(fds, nfds, 0) == ERROR)
		{
			std::cerr << RED << "Error: poll failed" << RESET << std::endl;
			return (1);
		}

		// server socket
		if (fds[0].revents != 0)
		{
			newClient = accept(sockfd, (sockaddr*)&addrClient, &len);
			if (newClient == ERROR)
			{
				std::cerr << RED << "Error: accept failed" << RESET << std::endl;
				return (1);
			}
			std::cout << "New connection" << std::endl;
			fds[nfds].fd = newClient;
			fds[nfds].events = POLLIN;
			nfds++;
		}

		// client sockets
		for (int i = 1; i < nfds; i++)
		{
			if (fds[i].revents == 0)
				continue ;

			if (fds[i].revents == 25)
			{
				std::cout << "connection closed with revent = 25" << std::endl;
				nfds--;
				close(fds[i].fd);
				if (i != nfds)
					fds[i].fd = fds[nfds].fd;
				fds[nfds].fd = 0;
				fds[i].revents = 0;
				continue ;
			}
			
			// verifier le nombre d'octects pour mettre un \0 a la fin
			if (recv(fds[i].fd, buffer, sizeof(buffer), 0) == ERROR)
			{
				std::cerr << RED << "Error: recv failed" << RESET << std::endl;
				return (1);
			}

			if (!buffer[0]) // ctrl+C
			{
				std::cout << "connection closed by ctrl+C" << std::endl;
				nfds--;
				close(fds[i].fd);
				if (i != nfds)
					fds[i].fd = fds[nfds].fd;
				fds[nfds].fd = 0;
				fds[i].revents = 0;
				continue ;
			}

			std::cout << "Received : " << buffer;

			for (int j = 1; j < nfds; j++)
				if (j != i)
					send(fds[j].fd, buffer, sizeof(buffer), 0);

			for (int i = 0; buffer[i]; i++)
				buffer[i] = 0;
		}
	}

	return (0);
}
