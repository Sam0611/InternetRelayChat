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

size_t Server::getClientId(const std::string name)
{
	size_t i;
	for (i = 0; i < _clients.size(); i++)
		if (!name.compare(_clients[i]->getName()))
			break ;
	return (i);
}

size_t Server::getChannelId(const std::string name)
{
	size_t i;
	for (i = 0; i < _channels.size(); i++)
		if (!name.compare(_channels[i]->getName()))
			break ;
	return (i);
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

	msg[1].append("\r\n");
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
			// check if exist
			int destfd = getFdByName(receivers[i]);
			if (destfd == ERROR) // if username not found
			{
				std::cerr << RED << "'" << receivers[i] << "' not found" << RESET << std::endl;
				continue ;
			}

			// formatting message content in one string
			std::string message = ":";
			message.append(_clients[id]->getName());
			message.append(" PRIVMSG ");
			message.append(msg[0]);
			message.append(" ");
			message.append(msg[1]);

			// send message content
			send(destfd, message.c_str(), message.length(), 0);
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
	std::string message;
	for (size_t i = 0; i < channelNames.size(); i++)
	{
		size_t j = getChannelId(channelNames[i]);
		if (j == _channels.size()) // channel doesn't exist
		{
			// create new channel
			_channels.push_back(new Channel(channelNames[i], _clients[id]->getName(), _clients[id]->getFd()));
			
			// if password, set mode +k with this password
			if (i < channelPasswords.size())
				_channels[j]->changeMode('+', 'k', channelPasswords[i]);
		}
		else // channel already exists
		{
			// check if mode invite only and client does'nt have invite
			if (_channels[j]->getMode('i') && !_clients[id]->hasInvite(channelNames[i]))
			{
				message = "You can't join ";
				message.append(channelNames[i]);
				message.append(" : invite only\n");
				send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
				continue ;
			}

			// check if key mode and password is wrong
			if (_channels[j]->getMode('k') && i >= channelPasswords.size() && !_channels[j]->checkPassword(channelPasswords[i]))
			{
				message = "You can't join ";
				message.append(channelNames[i]);
				message.append(" : wrong password\n");
				send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
				continue ;
			}

			// check if limit of users set and reached
			if (_channels[j]->getMode('l') && _channels[j]->limitReached())
			{
				message = "You can't join ";
				message.append(channelNames[i]);
				message.append(" : limit number of users reached\n");
				send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
				continue ;
			}

			_channels[j]->addUser(_clients[id]->getName(), _clients[id]->getFd());
		}

		_channels[j]->joinChannelMessage(_clients[id]->getFd(), _clients[id]->getName()); //messages when first connecting to channel
		rpl_topic(*_clients[id], *_channels[j]);
		rpl_namereply(*_clients[id], *_channels[j]);
		rpl_endofnames(*_clients[id], *_channels[j]);
		std::cerr << "testttt = " << _channels[j]->getName();
		_clients[id]->addChannel(channelNames[i]); // add channel in client class
		_clients[id]->removeInvite(channelNames[i]); // remove channel from client invites

	}
}

// PART #chan1,#chan2,#chan3
void Server::leave_channel(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain channel names)
	if (msg.size() != 1)
    {
        print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
        return ;
    }

	std::vector<std::string> channelNames = splitString(msg[0], ',');
	std::string errorMessage;

	for (size_t i = 0; i < channelNames.size(); i++)
	{
		if (!_clients[id]->isInChannel(channelNames[i]))
		{
			errorMessage = ": not in that channel\n";
			errorMessage.insert(0, channelNames[i]);
			send(_clients[id]->getFd(), errorMessage.c_str(), errorMessage.length(), 0);
			continue ;
		}

		_clients[id]->removeChannel(channelNames[i]);
		std::string message = "You left the channel ";
		message.append(channelNames[i]);
		message.append("\n");
		send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
		for (size_t j = 0; j < _channels.size(); j++)
		{
			if (!channelNames[i].compare(_channels[j]->getName()))
			{
				_channels[j]->removeUser(_clients[id]->getName());
				_channels[j]->sendMessage(_clients[id]->getName(), " left the channel\n");
				break ;
			}
		}
	}
}

// TOPIC #chan1 :new topic
void Server::view_or_change_topic(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain one channel name and may have a topic)
	if (msg.size() != 1 && msg.size() != 2)
    {
        print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
        return ;
    }

	// check if channel exists
	size_t i = getChannelId(msg[0]);
	if (i == _channels.size())
    {
        print_error_message(CHANNEL_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// view channel's topic
	if (msg.size() == 1)
	{
		std::string topic = _channels[i]->getName();
		topic.append(_channels[i]->getTopic());
		topic.append("\n");
		send(_clients[id]->getFd(), topic.c_str(), topic.length(), 0);
		return ;
	}

	// check if new topic begins with ':'
	if (msg[1][0] != ':')
    {
        print_error_message(INVALID_FORMAT, _clients[id]->getFd());
        return ;
    }

	// change topic if is operator
	if (_channels[i]->isOperator(_clients[id]->getName()))
		_channels[i]->setTopic(_clients[id]->getName(), msg[1]);
	else
        print_error_message(PERM_DENIED, _clients[id]->getFd());
}

// KICK #chan acomet :he is a bigboss
void Server::kick_from_channel(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain a channel name and a username, and may have a comment)
	if (msg.size() != 2 && msg.size() != 3)
    {
        print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
        return ;
    }

	// check if comment begins with ':'
	if (msg.size() == 3 && msg[2][0] != ':')
    {
        print_error_message(INVALID_FORMAT, _clients[id]->getFd());
        return ;
    }

	// check if channel exists
	size_t i = getChannelId(msg[0]);
	if (i == _channels.size())
    {
        print_error_message(CHANNEL_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// check if executor is operator
	if (!_channels[i]->isOperator(_clients[id]->getName()))
	{
		print_error_message(PERM_DENIED, _clients[id]->getFd());
		return ;
	}

	// check if user to kick exists and is in channel
	size_t j = getClientId(msg[1]);
	if (j == _clients.size() || !_channels[i]->isMember(msg[1]))
    {
        print_error_message(USER_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// removing
	_clients[j]->removeChannel(_channels[i]->getName());
	_channels[i]->removeUser(_clients[i]->getName());

	// get comment
	std::string reason;
	if (msg.size() == 3)
	{
		reason = " because ";
		reason.append(msg[2].substr(1));
	}

	// send message to channel members
	std::string message = " has been kicked from channel";
	message.append(reason);
	message.append("\n");
	_channels[i]->sendMessage(_clients[j]->getName(), message);

	// send private message to kicked user
	message = "You've been kicked from channel ";
	message.append(_channels[i]->getName());
	message.append(reason);
	message.append("\n");
	send(_clients[j]->getFd(), message.c_str(), message.length(), 0);
}

// INVITE acomet #chan
// There is no requirement that the channel the target user is being invited to must exist or be a valid channel
void Server::invite_to_channel(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain a username and a channel name)
	if (msg.size() != 2)
    {
        print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
        return ;
    }

	// check if user to invite exists
	size_t j = getClientId(msg[0]);
	if (j == _clients.size())
    {
        print_error_message(USER_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// check if channel exists and is invite only mode and sender is not operator
	size_t i = getChannelId(msg[1]);
	if (i != _channels.size() && _channels[i]->getMode('i') && !_channels[i]->isOperator(_clients[id]->getName()))
	{
        print_error_message(PERM_DENIED, _clients[id]->getFd());
        return ;
	}

	// save the invite in the invited client class
	_clients[j]->saveInvite(msg[1]);

	// send invite message to the invited
	std::string inviteMessage = " invited you to ";
	inviteMessage.insert(0, _clients[id]->getName());
	inviteMessage.append(msg[1]);
	inviteMessage.append("\n");
	send(_clients[id]->getFd(), inviteMessage.c_str(), inviteMessage.length(), 0);
}

// MODE #chan +i
void Server::change_mode(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain a channel name, the mode change and may have an optional arg (+k, +l, +o))
	if (msg.size() != 2 && msg.size() != 3)
    {
		print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
		return ;
    }

	// check if channel exists
	size_t i = getChannelId(msg[0]);
	if (i == _channels.size())
    {
        print_error_message(CHANNEL_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// check if client is operator of the channel
	if (!_channels[i]->isOperator(_clients[id]->getName()))
    {
        print_error_message(PERM_DENIED, _clients[id]->getFd());
        return ;
    }

	// check mode string size (must be = 2), check sign (+/-) and check mode (i/k/l/o/t)
	std::string	modes = "iklot";
	if (msg[1].size() != 2 || (msg[1][0] != '-' && msg[1][0] != '+')
		|| modes.find(msg[1][1]) == std::string::npos)
    {
        print_error_message(INVALID_FORMAT, _clients[id]->getFd());
        return ;
    }

	if (msg.size() == 2)
	{
		// check error number args
		if (!msg[1].compare("+k") || !msg[1].compare("+o") || !msg[1].compare("+l"))
		{
			print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
			return ;
		}

		if (!msg[1].compare("-o"))
			_channels[i]->changeMode(msg[1][0], msg[1][1], _clients[id]->getName());
		else
			_channels[i]->changeMode(msg[1][0], msg[1][1]);
		return ;
	}
	
	// msg size = 3

	// check error number args
	if (msg[1].compare("+k") && msg[1].compare("+o") && msg[1].compare("+l"))
	{
		print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
		return ;
	}

	// convert msg[2] to int when +l mode
	if (!msg[1].compare("+l"))
	{
		size_t len;
		std::istringstream ss(msg[2]);
		ss >> len;
		if (ss.fail() || !ss.eof())
		{
			print_error_message(INVALID_FORMAT, _clients[id]->getFd());
			return ;
		}
		_channels[i]->changeMode(msg[1][1], len);
		return ;
	}

	// if +k and password contains ',' or ':'
	if (!msg[1].compare("+k") 
		&& (msg[2].find(',') != std::string::npos || msg[2].find(':') != std::string::npos))
	{
		print_error_message(INVALID_PASSWORD, _clients[id]->getFd());
		return ;
	}

	// if +o and user is in the channel
	if (!msg[1].compare("+o") && !_channels[i]->isMember(msg[2])) 
	{
		print_error_message(USER_NOT_FOUND, _clients[id]->getFd());
		return ;
	}

	// change mode with +k or +o
	_channels[i]->changeMode(msg[1][0], msg[1][1], msg[2]);
}

// LIST
void Server::list_channels(std::vector<std::string> msg, int id)
{
	// the buffer size must be null
	if (msg.size())
    {
        print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
        return ;
    }

	// check if a channel exists
	if (!_channels.size())
		return ;

	// list channels name and topic
	std::string chanList;
	for (size_t i = 0; i < _channels.size(); i++)
	{
		chanList.append(_channels[i]->getName());
		chanList.append(_channels[i]->getTopic());
		chanList.append("\n");
	}
	send(_clients[id]->getFd(), chanList.c_str(), chanList.length(), 0);
}

void Server::process_commands(std::string input, int id)
{
	std::string str(input);
	std::string cmd[10] = {"PRIVMSG", "JOIN", "PART", "TOPIC", "INVITE", "KICK", "MODE", "QUIT", "LIST", "HELP"};
    std::vector<std::string> msg = splitString(str, ' ', ':');

    size_t i = 0;
    while (i < 10 && cmd[i].compare(msg[0]))
	{
        ++i;
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
            leave_channel(msg, id);
			break ;
        case 3: // TOPIC
            view_or_change_topic(msg, id);
			break ;
        case 4: // INVITE
            invite_to_channel(msg, id);
			break ;
        case 5: // KICK
            kick_from_channel(msg, id);
			break ;
        case 6: // MODE
            change_mode(msg, id);
			break ;
        case 7: // QUIT
            std::cout << "leaving irc" << std::endl;
			break ;
        case 8: // LIST
            list_channels(msg, id);
			break ;
        case 9: // HELP
            std::cout << "help" << std::endl;
			break ;
        default:
            print_error_message(COMMAND_NOT_FOUND, _clients[id]->getFd());
    }
}

// bool is_available_username(std::vector<Client *> clients, size_t id)
// {
// 	for (size_t i = 0; i < clients.size(); i++)
// 	{
// 		if (id == i || !clients[i]->info_set)
// 			continue ;
// 		clients[id]->compareNames(clients[i]->getName());
// 		if (clients[id]->getName().empty())
// 		{
// 			print_error_message(USERNAME_NOT_AVAILABLE, clients[id]->getFd());
// 			return (false);
// 		}
// 	}
// 	std::cout << clients[id]->getName() << " is logged" << std::endl;
// 	rpl_welcome(*clients[id]);
// 	return (true);
// }

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
			
			//test bigboss pour tronc
			int msglen = recv(fds[i].fd, buffer, sizeof(buffer), 0);
			std::cerr << "msglen = " << msglen << std::endl;
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

				//bigboss test
			std::cerr << "test : " << buffer << std::endl;

			// seperate multiples commands from one buffer
			std::vector<std::string> cmds = splitString(buffer, '\n');

			// check each cmd in buffer
			for (size_t k = 0; k < cmds.size(); k++)
			{
				//removing '\r'
				if (cmds[k][cmds[k].size() -1] == '\r')
					cmds[k].erase(cmds[k].size() - 1); // remove \n at the end

				if (!_clients[i - FIRST_CLIENT]->pass_set)// process PASS
					_clients[i - FIRST_CLIENT]->check_password_input(cmds[k], _password);
				else if (!_clients[i - FIRST_CLIENT]->nick_set || !_clients[i -FIRST_CLIENT]->user_set) // process NICK / USER
					_clients[i - FIRST_CLIENT]->identifying(cmds[k], _clients);
				else
				{
					std::cout << "Received : " << buffer;
					// process IRC commands
					process_commands(cmds[k].c_str(), i - FIRST_CLIENT);
				}
			}

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
