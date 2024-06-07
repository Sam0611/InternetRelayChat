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

int	main(int ac, char **av)
{
	if (ac != 2) // 3 mdp
	{
        std::cerr << RED << "Error: Wrong number of arguments" << RESET << std::endl;
		return (1);
	}

	// check av[1] is number and range is valid
	long port = strtol(av[1], NULL, 0);
	if (port <= 0)
	{
        std::cerr << RED << "Error: Invalid port number" << RESET << std::endl;
		return (1);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == ERROR)
	{
        std::cerr << RED << "Error: Could'nt initialize socket" << RESET << std::endl;
		return (1);
	}

	sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == ERROR)
	{
        std::cerr << RED << "Error: Could'nt bind socket" << RESET << std::endl;
		return (1);
	}

	if (listen(sockfd, SOMAXCONN) == ERROR)
	{
        std::cerr << RED << "Error: listen failed" << RESET << std::endl;
		return (1);
	}

	sockaddr_in addrClient;
	socklen_t len = sizeof(addrClient);
	int newClient = accept(sockfd, (sockaddr*)&addrClient, &len);
	if (newClient == ERROR)
	{
        std::cerr << RED << "Error: accept failed" << RESET << std::endl;
		return (1);
	}

	char buffer[1024];
	if (recv(newClient, buffer, sizeof(buffer), 0) == ERROR)
	{
        std::cerr << RED << "Error: recv failed" << RESET << std::endl;
		return (1);
	}


	std::cout << "Received : " << buffer << std::endl;

	close(sockfd);
	close(newClient);

	return (0);
}
