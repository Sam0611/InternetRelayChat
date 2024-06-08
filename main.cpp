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

#include <iostream>
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in htons
#include <stdlib.h> // strtol
#include <poll.h> // poll
#include <unistd.h> // close

#define RED "\033[31;01m"
#define RESET "\033[00m"
#define ERROR -1
#define VALID_PORT_MIN 1024
#define VALID_PORT_MAX 65535

int	main(int ac, char **av)
{
	if (ac != 2) // 3 mdp
	{
        std::cerr << RED << "Error: Wrong number of arguments" << RESET << std::endl;
		return (1);
	}

	// check av[1] is number and range is valid
	long port = strtol(av[1], NULL, 0);
	if (port < VALID_PORT_MIN || port > VALID_PORT_MAX)
	{
        std::cerr << RED << "Error: Invalid port number" << RESET << std::endl;
		return (1);
	}

	// AF_INET = IPv4, SOCK_STREAM = TCP, IPPROTO_TCP = TCP
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == ERROR)
	{
        std::cerr << RED << "Error: Could'nt initialize socket" << RESET << std::endl;
		return (1);
	}

	// INADDR_ANY any IPv4 local host address
	sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	// attention erreur quand le port est utilise 2 fois de suite
	// casting en C++ avec <reinterpret_cast> ou autre
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == ERROR)
	{
        std::cerr << RED << "Error: Could'nt bind socket" << RESET << std::endl;
		return (1);
	}

	// non bloquante
	if (listen(sockfd, SOMAXCONN) == ERROR)
	{
        std::cerr << RED << "Error: listen failed" << RESET << std::endl;
		return (1);
	}

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

		for (int i = 0; i < nfds; i++)
		{
			if (fds[i].revents == 0)
				continue ;

			// if (fds[i].revents == POLLIN)
			// std::cout << i << " : " << fds[i].revents << std::endl;

			// if (fds[i].revents != POLLIN)
			// 	break ;

			if (fds[i].fd == sockfd)
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
			else
			{
				if (fds[i].revents == 25)
				{
					std::cout << "connection closed" << std::endl;
					close(fds[i].fd);
					fds[i].fd = fds[nfds].fd;
					fds[nfds].fd = 0;
					fds[i].revents = 0;
					nfds--;
					continue ;
				}
				
				if (recv(fds[i].fd, buffer, sizeof(buffer), 0) == ERROR)
				{
					std::cerr << RED << "Error: recv failed" << RESET << std::endl;
					return (1);
				}
				std::cout << "Received : " << buffer;

				for (int j = 1; j < nfds; j++)
					if (j != i)
						send(fds[j].fd, buffer, sizeof(buffer), 0);

				for (int i = 0; buffer[i]; i++)
					buffer[i] = 0;
			}
		}
	}

	// sockaddr_in addrClient;
	// socklen_t len = sizeof(addrClient);
	// // accept = bloquant
	// int newClient = accept(sockfd, (sockaddr*)&addrClient, &len);
	// if (newClient == ERROR)
	// {
    //     std::cerr << RED << "Error: accept failed" << RESET << std::endl;
	// 	return (1);
	// }
	// else
	// 	std::cout << "New connection" << std::endl;

	// char buffer[1024];
	// // recv = bloquant
	// if (recv(newClient, buffer, sizeof(buffer), 0) == ERROR)
	// {
    //     std::cerr << RED << "Error: recv failed" << RESET << std::endl;
	// 	return (1);
	// }
	// std::cout << "Received : " << buffer << std::endl;

	close(sockfd);
	close(newClient);

	return (0);
}
