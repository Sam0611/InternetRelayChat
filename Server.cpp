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
	addr.sin_addr.s_addr = INADDR_ANY; // any IPv4 local host address
    // addr.sin_addr.s_addr = inet_addr(_host.c_str());
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

			// formatting message content in one string
			std::string message = ":";
			message.append(_clients[id]->getName());
            message.append(" PRIVMSG ");
			message.append(receivers[i]);
            message.append(" ");
			message.append(msg[1]);

			// send message to everyone in the channel
			for (size_t j = 0; j < _channels.size(); j++)
				if (!receivers[i].compare(_channels[j]->getName()))
					_channels[j]->sendMessageloop(message);
		}
		else // if receiver is a nickname
		{
			// check if exist
			int destfd = getFdByName(receivers[i]);
			if (destfd == ERROR) // if username not found
			{
				std::cerr << RED << "'" << receivers[i] << "' not found" << RESET << std::endl;
				print_error_message(NO_SUCH_NICK, _clients[id]->getFd());
				continue ;
			}

			// formatting message content in one string
			std::string message = ":";
			message.append(_clients[id]->getName());
			message.append(" PRIVMSG ");
			message.append(receivers[i]);
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
				message = ":";
				message.append(SERVER);
				message.append(" 473 ");
				message.append(_clients[id]->getName());
				message.append(" ");
				message.append(channelNames[i]);
				message.append(" :Cannot join channel (+i)\r\n");
				send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
				continue ;
			}

			// check if key mode and password is wrong
			if (_channels[j]->getMode('k') && (i >= channelPasswords.size() || !_channels[j]->checkPassword(channelPasswords[i])))
			{
				message = ":";
				message.append(SERVER);
				message.append(" 475 ");
				message.append(_clients[id]->getName());
				message.append(" ");
				message.append(channelNames[i]);
				message.append(" :Cannot join channel (+k)\r\n");
				send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
				continue ;
			}

			// check if limit of users set and reached
			if (_channels[j]->getMode('l') && _channels[j]->limitReached())
			{
				message = ":";
				message.append(SERVER);
				message.append(" 471 ");
				message.append(_clients[id]->getName());
				message.append(" ");
				message.append(channelNames[i]);
				message.append(" :Cannot join channel (+l)\r\n");
				send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
				continue ;
			}

			_channels[j]->addUser(_clients[id]->getName(), _clients[id]->getFd());
		}

		_channels[j]->joinChannelMessage(_clients[id]->getFd(), _clients[id]->getName()); //messages when first connecting to channel
		rpl_topic(_clients[id], _channels[j]);
		rpl_namereply(_clients[id], _channels[j]);
		rpl_endofnames(_clients[id], _channels[j]);
		_clients[id]->addChannel(channelNames[i]); // add channel in client class
		_clients[id]->removeInvite(channelNames[i]); // remove channel from client invites

	}
}

// PART #chan1,#chan2,#chan3
void Server::leave_channel(std::vector<std::string> msg, int id)
{
	// check the buffer size (must contain channel names)
	if (msg.size() < 1 || (msg.size() > 1 && msg[1][0] != ':'))
    {
        print_error_message(INVALID_FORMAT, _clients[id]->getFd());
        return ;
    }

	std::vector<std::string> channelNames = splitString(msg[0], ',');
	std::string errorMessage;

	for (size_t i = 0; i < channelNames.size(); i++)
	{
		if (!_clients[id]->isInChannel(channelNames[i]))
		{
			print_error_message(NOT_IN_THE_CHANEL, _clients[id]->getFd());
			continue ;
		}

		_clients[id]->removeChannel(channelNames[i]);
		std::string message = ":";
		message.append(_clients[id]->getName());
		message.append(" PART ");
		message.append(channelNames[i]);
		if (msg.size() == 2 && !msg[1].empty())
		{
			message.append(" ");
			message.append(msg[1]);
		}
		message.append("\r\n");
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

	// check if channel exists and client is in it
	size_t i = getChannelId(msg[0]);
	if (i == _channels.size() || !_channels[i]->isMember(_clients[id]->getName()))
    {
        print_error_message(CHANNEL_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// view channel's topic
	if (msg.size() == 1)
	{
		rpl_topic(_clients[id], _channels[i]);
		return ;
	}

	// check if new topic begins with ':'
	if (msg[1][0] != ':')
    {
        print_error_message(INVALID_FORMAT, _clients[id]->getFd());
        return ;
    }

	// change topic if is operator
	if (!_channels[i]->isOperator(_clients[id]->getName()))
	{
        print_error_message(PERM_DENIED, _clients[id]->getFd());
		return;
	}
	_channels[i]->setTopic(_clients[id]->getName(), msg[1]);
	std::string message = ":";
	message.append(_clients[id]->getName());
	message.append(" PRIVMSG ");
	message.append(_channels[i]->getName());
	message.append(" changed topic : ");
	message.append(msg[1].substr(1));
	message.append("\r\n");
	_channels[i]->sendMessageloop(message);
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
	if (!_channels[i]->isOperator(_clients[id]->getName()) || _channels[i]->isOperator(msg[1]))
	{
		print_error_message(PERM_DENIED, _clients[id]->getFd());
		return ;
	}

	// check if user to kick exists and is in channel
	size_t j = getClientId(msg[1]);
	if (j == _clients.size() || !_channels[i]->isMember(msg[1]) || _channels[i]->isOperator(msg[1]))
    {
        print_error_message(USER_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// get comment
	std::string reason;
	if (msg.size() == 3)
	{
		reason = " because ";
		reason.append(msg[2].substr(1));
	}

	// send message to channel members
	std::string message = ":";
	message.append(_clients[id]->getName());
	message.append(" KICK ");
	message.append(_channels[i]->getName());
	message.append(" ");
	message.append(_clients[j]->getName());
	message.append(" ");
	message.append(reason);
	message.append("\r\n");
	_channels[i]->sendMessageloop(message);

	// removing
	_clients[j]->removeChannel(_channels[i]->getName());
	_channels[i]->removeUser(_clients[j]->getName());
}

// INVITE acomet #chan //he is a bigoss
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

	// send rpl_invite and invite message to the invited
	rpl_inviting(_clients[id], _clients[j], _channels[getChannelId(msg[1])]);
	std::string inviteMessage = ":";
	inviteMessage.append(_clients[id]->getName());
	inviteMessage.append(" INVITE ");
	inviteMessage.append(_clients[j]->getName());
	inviteMessage.append(" :");
	inviteMessage.append(msg[1]);
	inviteMessage.append("\r\n");
	_channels[getChannelId(msg[1])]->sendMessageloopExcept(inviteMessage, _clients[id]->getName());
	send(_clients[j]->getFd(), inviteMessage.c_str(), inviteMessage.length(), 0);
}

// MODE #chan +kl-i+o mdp 3 acomet
void Server::change_mode(std::vector<std::string> msg, int id)
{
	std::string	modes = MODES;

	// check the buffer size
	if (!msg.size())
    {
		print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
		return ;
    }

	// check if channel exists and client is in it
	size_t i = getChannelId(msg[0]);
	if (i == _channels.size() || !_channels[i]->isMember(_clients[id]->getName()))
    {
        print_error_message(CHANNEL_NOT_FOUND, _clients[id]->getFd());
        return ;
    }

	// show channel modes
	if (msg.size() == 1)
    {
        // _channels[i]->showModes();
        std::string message = ":";
        message.append(SERVER);
        message.append(" 324 ");
        message.append(_clients[id]->getName());
        message.append(" ");
        message.append(_channels[i]->getName());
        message.append(" +");
        for (size_t j = 0; j < modes.size(); j++)
        {

            if (_channels[i]->getMode(modes[j]))
                message.push_back(modes[j]);
        }
        message.append("\r\n");
        send(_clients[id]->getFd(), message.c_str(), message.length(), 0);
        return ;
    }

	// check if client is operator of the channel
	if (!_channels[i]->isOperator(_clients[id]->getName()))
    {
        print_error_message(PERM_DENIED, _clients[id]->getFd());
        return ;
    }

    bool activate = true;
	size_t argModes = 0;
	size_t len = 0;

	// parsing : no double mode, no invalid mode, number of arguments, no invalide limit users
    for (size_t j = 0; j < msg[1].size(); j++)
	{
        if (msg[1][j] == '-')
            activate = false;
        else if (msg[1][j] == '+')
            activate = true;
		else if (modes.find(msg[1][j]) == std::string::npos || msg[1].substr(j + 1).find(msg[1][j]) != std::string::npos)
        {
            print_error_message(INVALID_FORMAT, _clients[id]->getFd());
            return ;
        }

		if (activate && msg[1][j] == 'l')
		{
			if (msg.size() <= argModes + 2)
			{
				print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
				return ;
			}
			std::istringstream ss(msg[2 + argModes]);
			ss >> len;
			if (ss.fail() || !ss.eof())
			{
				print_error_message(INVALID_FORMAT, _clients[id]->getFd());
				return ;
			}
		}

		if (_channels[i]->needArgMode(activate, msg[1][j]))
			argModes++;
	}
	if (argModes != msg.size() - 2)
	{
		print_error_message(WRONG_ARG_NUMBER, _clients[id]->getFd());
		return ;
	}

	// apply change of mode
    for (size_t j = 0; j < msg[1].size(); j++)
    {
        if (msg[1][j] == '-')
            activate = false;
        else if (msg[1][j] == '+')
            activate = true;
        else if (_channels[i]->needArgMode(activate, msg[1][j]))
		{
			if (msg[1][j] == 'l')
				_channels[i]->changeMode(msg[1][j], len, _clients[id], msg[2]);
			else if (msg[1][j] == 'k')
				_channels[i]->changeMode(activate, msg[1][j], msg[2]);
			else // +o
			{
				size_t opId = getClientId(msg[2]);
				if (opId != _clients.size())
					_channels[i]->changeOperator(activate, msg[2]);
			}
			msg.erase(msg.begin() + 2);
		}
		else if (msg[1][j] == 'o') // -o
			_channels[i]->changeOperator(activate, _clients[id]->getName());
		else
			_channels[i]->changeMode(activate, msg[1][j]);
    }
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
		chanList.append("\r\n");
	}
	send(_clients[id]->getFd(), chanList.c_str(), chanList.length(), 0);
}

// QUIT <quit message>
void Server::quit(std::string msg, int id)
{
    if (!msg.empty())
        std::cout << "disconnexion :" << msg << std::endl;
    else
        std::cout << "disconnected" << std::endl;

    _nfds--;
    close(_fds[id + FIRST_CLIENT].fd);
    if (id + FIRST_CLIENT != _nfds)
        _fds[id + FIRST_CLIENT].fd = _fds[_nfds].fd;
    _fds[_nfds].fd = 0;
    _fds[id + FIRST_CLIENT].revents = 0;
    
    delete _clients[id];
    _clients.erase(_clients.begin() + (id));
}

void Server::process_commands(std::string input, int id)
{
	std::string str(input);
	std::string cmd[11] = {"PRIVMSG", "JOIN", "PART", "TOPIC", "INVITE", "KICK", "MODE", "QUIT", "LIST", "WHO"};
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
            quit(msg[0], id);
			break ;
        case 8: // LIST
            list_channels(msg, id);
			break ;
        case 9: // WHO silenced
			break ;
        default:
            print_error_message(COMMAND_NOT_FOUND, _clients[id]->getFd());
    }
}

void stop_server(int signal)
{
	(void)signal;
}

int Server::startServer(void)
{
	// set poll variables
	for (int i = 0; i < MAX_CONNEXION; i++)
	{
		_fds[i].events = POLLIN;
		_fds[i].revents = 0;
	}
	_fds[0].fd = 0;
	_fds[1].fd = _socket;
	_nfds = 2;

	// initialize client
	int fdClient = -1;
	sockaddr_in addrClient;
	socklen_t len = sizeof(addrClient);

	// variables for received content
	std::string input; // stdin
	char buffer[BUFFER_SIZE]; // clients
	for (size_t i = 0; i < sizeof(buffer); i++)
		buffer[i] = 0;
	
	signal(SIGINT, stop_server);

	while (1)
	{
		if (poll(_fds, _nfds, 0) == ERROR)
			break ;

		// stdin
		if (_fds[0].revents != 0)
		{
			std::cin >> input;
            if (input == "exit")
				break ;
		}

		// server socket
		if (_fds[1].revents != 0)
		{
			// receiving new connection
			fdClient = accept(_socket, (sockaddr*)&addrClient, &len);
			if (fdClient == ERROR)
			{
				std::cerr << RED << "Error: accept failed" << RESET << std::endl;
				break ;
			}

			// check if number of max connection has been reached
			if (_nfds == MAX_CONNEXION)
			{
				std::cerr << RED << "Error: number of max connection reached" << RESET << std::endl;
				close(fdClient);
				fdClient = -1;
				continue ;
			}

			// add new client
			std::cout << "New connection" << std::endl;
			_clients.push_back(new Client(fdClient));
			_fds[_nfds].fd = fdClient;
			_nfds++;
		}

		// client sockets
		for (int i = FIRST_CLIENT; i < _nfds; i++)
		{
			if (_fds[i].revents == 0)
				continue ;

			if (_fds[i].revents != POLLIN)
			{
                quit("", i - FIRST_CLIENT);
				continue ;
			}
			
			//test bigboss pour trunc
			int msglen = recv(_fds[i].fd, buffer, sizeof(buffer), 0);
			std::cerr << "msglen = " << msglen << std::endl;
			if (msglen == ERROR)
			{
				std::cerr << RED << "Error: recv failed" << RESET << std::endl;
				return (1);
			}

			if (!buffer[0]) // ctrl+C
			{
                quit("", i - FIRST_CLIENT);
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
				{
					_clients[i - FIRST_CLIENT]->identifying(cmds[k], _clients);
					if (_clients[i - FIRST_CLIENT]->nick_set && _clients[i - FIRST_CLIENT]->user_set)
						rpl_welcome(_clients[i - FIRST_CLIENT]);
				}
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
	for (int i = FIRST_CLIENT; i < _nfds; i++)
		if (_fds[i].fd > 0)
			close(_fds[i].fd);

	return (0);
}