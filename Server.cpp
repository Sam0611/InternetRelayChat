/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 18:05:08 by sam               #+#    #+#             */
/*   Updated: 2024/06/09 18:05:10 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h> // inet_addr
#include "Server.hpp"

// verif if this the goods values [to do]
#define VALID_PORT_MIN 1024
#define VALID_PORT_MAX 65535

Server::Server() : _host("127.0.0.1"), _port(-1), _socket(-1)
{
}

Server::~Server()
{
    if (_socket > 0)
        close(_socket);
}

int Server::createServer(char *input)
{
    _port = strtol(input, NULL, 0);
	if (_port < VALID_PORT_MIN || _port > VALID_PORT_MAX)
	{
        std::cerr << RED << "Error: Invalid port number" << RESET << std::endl;
		return (ERROR);
	}

    // AF_INET = IPv4, SOCK_STREAM = TCP, IPPROTO_TCP = TCP
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == ERROR)
	{
        std::cerr << RED << "Error: Couldn't initialize socket" << RESET << std::endl;
		return (ERROR);
	}

	sockaddr_in addr;
	// addr.sin_addr.s_addr = INADDR_ANY; // any IPv4 local host address
    addr.sin_addr.s_addr = inet_addr(_host.c_str());
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);

	// casting en C++ avec <reinterpret_cast> ou autre
	if (bind(_socket, (struct sockaddr*)&addr, sizeof(addr)) == ERROR)
	{
        std::cerr << RED << "Error: Couldn't bind socket" << RESET << std::endl;
		return (ERROR);
	}

	// non bloquante
	if (listen(_socket, SOMAXCONN) == ERROR)
	{
        std::cerr << RED << "Error: listen failed" << RESET << std::endl;
		return (ERROR);
	}

    return (_socket);
}

int Server::startServer(void)
{
	// check if POLLOUT is a relevant good events arg [to do]
	// protect the max connexion number [to do]
	struct pollfd fds[MAX_CONNEXION];
	for (int i = 0; i < MAX_CONNEXION; i++)
	{
		fds[i].events = POLLIN;
		fds[i].revents = 0;
	}
	fds[0].fd = 0;
	fds[1].fd = _socket;
	int nfds = 2;
	int newClient = -1;
	sockaddr_in addrClient;
	socklen_t len = sizeof(addrClient);
	bool msgBeginning = true;
	char buffer[BUFFER_SIZE];
	std::string input;

	// check if useful to "memset"
	for (size_t i = 0; i < sizeof(buffer); i++)
		buffer[i] = 0;

	
	while (1)
	{
		if (poll(fds, nfds, 0) == ERROR)
		{
			std::cerr << RED << "Error: poll failed" << RESET << std::endl;
			return (1);
		}

		// stdin
		if (fds[0].revents != 0)
		{
			std::cin >> input;
            if (input == "exit")
				break ;
		}

		// server socket
		if (fds[1].revents != 0)
		{
			newClient = accept(_socket, (sockaddr*)&addrClient, &len);
			if (newClient == ERROR)
			{
				std::cerr << RED << "Error: accept failed" << RESET << std::endl;
				return (1);
			}
			std::cout << "New connection" << std::endl;
			fds[nfds].fd = newClient;
			nfds++;
		}

		// client sockets
		for (int i = 2; i < nfds; i++)
		{
			if (fds[i].revents == 0)
				continue ;

			if (fds[i].revents != POLLIN)
			{
				std::cout << "connection closed" << std::endl;
				nfds--;
				close(fds[i].fd);
				if (i != nfds)
					fds[i].fd = fds[nfds].fd;
				fds[nfds].fd = 0;
				fds[i].revents = 0;
				continue ;
			}
			
			int msglen = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			if (msglen == ERROR)
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

			// if message is \n only, then do nothing
			if (msgBeginning && buffer[0] == '\n')
			{
				buffer[0] = 0;
				continue ;
			}

			// put a \0 at the end of message
			// buffer[msglen] = 0;

			// print message in server side
			if (msgBeginning)
			{
				std::cout << "Received : ";
				msgBeginning = false;
			}
			std::cout << buffer;

			// print message to all clients
			for (int j = 2; j < nfds; j++)
				if (j != i)
					send(fds[j].fd, buffer, sizeof(buffer), 0);

			// if message ends with \n, the next will be starting
			if (buffer[msglen - 1] == '\n')
				msgBeginning = true;

			for (int j = 0; buffer[j]; j++)
				buffer[j] = 0;
		}
	}
	return (0);
}