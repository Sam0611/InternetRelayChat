/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 18:05:08 by sam               #+#    #+#             */
/*   Updated: 2024/06/16 18:10:22 by smalloir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h> // inet_addr
#include "Server.hpp"

// verif if this the goods values [to do]
#define VALID_PORT_MIN 1024
#define VALID_PORT_MAX 65535

Server::Server(std::string password) : _host("127.0.0.1"), _password(password), _port(-1), _socket(-1)
{
}

Server::~Server()
{
	for (size_t i = 0; i < _clients.size(); i++)
		delete _clients[i];
	for (size_t i = 0; i < _channels.size(); i++)
		delete _channels[i];
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

int Server::getFdByName(const std::string name)
{
	for (size_t i = 0; i < _clients.size(); i++)
		if (!name.compare(_clients[i]->getName()))
			return (_clients[i]->getFd());

	return (ERROR);
}

void Server::send_private_message(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain receiver and message content)
	if (msg.size() < 2)
    {
        print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
        return ;
    }

	// check message format (must begin with ':')
	if (msg[1][0] != ':')
	{
        print_error_message(INVALID_FORMAT, _clients[id]->getFd());
		return ;
	}

	// get receivers separated by commas
	std::vector<std::string> receivers = splitString(msg[0], ',');

	std::string msgContent;
	msg[1].push_back('\n');
	for (size_t i = 0; i < receivers.size(); i++) // for each receiver
	{
		if (receivers[i][0] == '#') // if receiver is a channel
		{
			// check sender is in it
			if (!_clients[id]->isInChannel(receivers[i]))
			{
				std::cerr << RED << "'" << receivers[i] << "' not joined" << RESET << std::endl;
				continue ;
			}

			// send message to everyone in the channel
			for (size_t j = 0; j < _channels.size(); j++)
				if (!receivers[i].compare(_channels[j]->getName()))
					_channels[j]->sendMessage(_clients[id]->getName(), msg[1]);
		}
		else // if receiver is a nickname
		{
			// check if nickname is the same as the client sending message
			if (!receivers[i].compare(_clients[id]->getName()))
				continue ;

			// check if exist
			int destfd = getFdByName(receivers[i]);
			if (destfd == ERROR) // if username not found
			{
				std::cerr << RED << "'" << receivers[i] << "' not found" << RESET << std::endl;
				continue ;
			}

			// formatting message content in one string
			msgContent = _clients[id]->getName();
			msgContent.append(msg[1]);

			// send message content
			send(destfd, msgContent.c_str(), msgContent.length(), 0);
		}
	}
}

// JOIN #chan1,#chan2,#chan3 mdp1,mdp2
void Server::join_channel(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain at least a channel name and may have password)
	if (msg.size() != 1 && msg.size() != 2)
    {
        print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
        return ;
    }

	std::vector<std::string> channelNames;
	std::vector<std::string> channelPasswords;

	channelNames = splitString(msg[0], ',');
	if (msg.size() == 2)
		channelPasswords = splitString(msg[1], ',');

	// check if there is more passwords than channel names
	if (channelPasswords.size() > channelNames.size())
    {
        print_error_message(TOO_MANY_KEYS, _clients[id]->getFd());
        return ;
    }

	// parsing channels name
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		// check if channel name begins with '#'
		if (channelNames[i][0] != '#')
		{
			print_error_message(INVALID_FORMAT, _clients[id]->getFd());
			return ;
		}

		// check if valid name, must not contain ‘\a’ (ctrl+G)
		if (channelNames[i].find('\a') != std::string::npos)
		{
			print_error_message(INVALID_NAME, _clients[id]->getFd());
			return ;
		}
	}

	// create and join channels
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		// check if channel exists
		size_t j;
		for (j = 0; j < _channels.size(); j++)
			if (!channelNames[i].compare(_channels[j]->getName()))
				break ;
		
		if (j == _channels.size())
		{
			// create new channel
			_channels.push_back(new Channel(channelNames[i], _clients[id]->getName(), _clients[id]->getFd()));
			
			// if password, set mode +k with this password
		}
		else
		{
			// check mode i/k/l (invite only, need password, limit of user set and reached)
			// passwords (mdp[1])
			_channels[j]->addUser(_clients[id]->getName(), _clients[id]->getFd());
		}

		// add channel in client class
		_clients[id]->addChannel(channelNames[i]);
	}
}

void Server::process_commands(char *input, int id)
{
	std::string str(input);
    str.erase(str.size() - 1); // remove \n at the end
	std::string cmd[10] = {"PRIVMSG", "JOIN", "PART", "TOPIC", "INVITE", "KICK", "MODE", "QUIT", "LIST", "HELP"};
    std::vector<std::string> msg = splitString(str, ' ', ':');

    size_t i = 0;
    while (i < 10 && cmd[i].compare(msg[0]))
	{
        i++;
	}

	// deleting part of msg containing command name
	msg.erase(msg.begin());

    switch(i)
    {
        case 0: // PRIVMSG
			send_private_message(msg, id);
			break ;
        case 1: // JOIN
            join_channel(msg, id);
			break ;
        case 2: // PART
            std::cout << "leaving chan" << std::endl;
			break ;
        case 3: // TOPIC
            std::cout << "topic" << std::endl;
			break ;
        case 4: // INVITE
            std::cout << "invite" << std::endl;
			break ;
        case 5: // KICK
            std::cout << "kick" << std::endl;
			break ;
        case 6: // MODE
            std::cout << "changing mode" << std::endl;
			break ;
        case 7: // QUIT
            std::cout << "leaving irc" << std::endl;
			break ;
        case 8: // LIST
            std::cout << "list of chan" << std::endl;
			break ;
        case 9: // HELP
            std::cout << "help" << std::endl;
			break ;
        default:
            print_error_message(COMMAND_NOT_FOUND, _clients[id]->getFd());
    }
}

bool is_available_username(std::vector<Client *> clients, size_t id)
{
	for (size_t i = 0; i < clients.size(); i++)
	{
		if (id == i || !clients[i]->info_set)
			continue ;
		clients[id]->compareNames(clients[i]->getName());
		if (clients[id]->getName().empty())
		{
			print_error_message(USERNAME_NOT_AVAILABLE, clients[id]->getFd());
			return (false);
		}
	}
	std::cout << clients[id]->getName() << " is logged" << std::endl;
	return (true);
}

int Server::startServer(void)
{
	// check if POLLOUT is a relevant good events arg [to do]
	// protect the max connexion number [to do]

	// set poll variables
	struct pollfd fds[MAX_CONNEXION];
	for (int i = 0; i < MAX_CONNEXION; i++)
	{
		fds[i].events = POLLIN;
		fds[i].revents = 0;
	}
	fds[0].fd = 0;
	fds[1].fd = _socket;
	int nfds = 2;

	// initialize client
	int fdClient = -1;
	sockaddr_in addrClient;
	socklen_t len = sizeof(addrClient);

	// variables for received content
	// bool msgBeginning = true;
	std::string input; // stdin
	char buffer[BUFFER_SIZE]; // clients
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
			// receiving new connection
			fdClient = accept(_socket, (sockaddr*)&addrClient, &len);
			if (fdClient == ERROR)
			{
				std::cerr << RED << "Error: accept failed" << RESET << std::endl;
				return (1);
			}

			// check if number of max connection has been reached
			if (nfds == MAX_CONNEXION)
			{
				std::cerr << RED << "Error: number of max connection reached" << RESET << std::endl;
				close(fdClient);
				fdClient = -1;
				continue ;
			}

			// add new client
			std::cout << "New connection" << std::endl;
			_clients.push_back(new Client(fdClient));
			fds[nfds].fd = fdClient;
			nfds++;
		}

		// client sockets
		for (int i = FIRST_CLIENT; i < nfds; i++)
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
				delete _clients[i - FIRST_CLIENT];
				_clients.erase(_clients.begin() + (i - FIRST_CLIENT));
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
				delete _clients[i - FIRST_CLIENT];
				_clients.erase(_clients.begin() + (i - FIRST_CLIENT));
				continue ;
			}

			if (!_clients[i - FIRST_CLIENT]->info_set)
			{
				// process input as command (PASS / NICK / USER)
				_clients[i - FIRST_CLIENT]->log_in(buffer, _password);

				// if client fullfilled info -> check if username is available
				if (_clients[i - FIRST_CLIENT]->check_informations())
					_clients[i - FIRST_CLIENT]->info_set = is_available_username(_clients, i - FIRST_CLIENT);
			}
			else
			{
				std::cout << "Received : " << buffer;
				// process IRC commands
				process_commands(buffer, i - FIRST_CLIENT);
			}
			

			


			// if message is \n only, then do nothing
			// if (msgBeginning && buffer[0] == '\n')
			// {
			// 	buffer[0] = 0;
			// 	continue ;
			// }

			// put a \0 at the end of message
			// buffer[msglen] = 0;

			// print message in server side
			// if (msgBeginning)
			// {
			// 	std::cout << "Received : ";
			// 	msgBeginning = false;
			// }
			// std::cout << buffer;

			// print message to all clients
			// for (int j = FIRST_CLIENT; j < nfds; j++)
			// 	if (j != i)
			// 		send(fds[j].fd, buffer, sizeof(buffer), 0);

			// if message ends with \n, the next will be starting
			// if (buffer[msglen - 1] == '\n')
			// 	msgBeginning = true;

			for (int j = 0; buffer[j]; j++)
				buffer[j] = 0;
		}
	}

	// closing fd clients still connected when server ends
	for (int i = FIRST_CLIENT; i < nfds; i++)
		if (fds[i].fd > 0)
			close(fds[i].fd);

	return (0);
}
