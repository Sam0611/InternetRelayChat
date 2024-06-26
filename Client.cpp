/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 13:26:33 by sam               #+#    #+#             */
/*   Updated: 2024/06/13 13:26:34 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : pass_set(false), nick_set(false), user_set(false), _fd(-1)
{
}

Client::Client(int fd) : pass_set(false), nick_set(false), user_set(false), _fd(fd)
{
}

Client::~Client()
{
}

void Client::check_password_input(std::string input, std::string password)
{
    std::vector<std::string> msg = splitString(input, ' ');

    if (msg[0].compare("PASS"))
    {
        print_error_message(PASSWORD_MISSMATCH, _fd);
        return;
    }

    if (msg.size() < 2)
    {
        print_error_message(NEED_MORE_PARAM, _fd);
        return ;
    }

    if (!msg[msg.size() -1].compare(password))
    {
        pass_set = true;
        
        std::cerr << "password entered" << std::endl; // bigboss test a suppr
    }
    else
        print_error_message(PASSWORD_MISSMATCH, _fd);
}

void Client::set_nickname(std::vector<std::string> msg, std::vector<Client *> clients)
{
    if (msg.size() < 2)
    {
        print_error_message(NO_NICKNAME_GIVEN, _fd);
        return ;
    }

    if (msg.size() > 2)
    {
        print_error_message(WRONG_ARG_NUMBER, _fd);
        return ;
    }

    if (msg[1] == _name)
    {
        print_error_message(NICKNAME_ALREADY_SET, _fd);
        return ;
    }

    if (msg[1].find_first_of(",#$@:") != std::string::npos)
    {
        print_error_message(NICKNAME_FORBIDDEN, _fd);
        return ;
    }

    for (size_t i = 0; i < clients.size(); i++)
    {
        if (!msg[1].compare(clients[i]->_name))
        {
            print_error_message(NICKNAME_ALREADY_USE, _fd);
            return ;
        }
    }

    _name = msg[1];
    nick_set = true;
    std::cerr << "nickname entered" << std::endl; // bigboss test a suppr
}

void Client::set_usernames(std::vector<std::string> msg)
{
    if (_username.size())
    {
        print_error_message(PARAMETER_ALREADY_SET, _fd);
        return ;
    }

    if (msg.size() != 5)
    {
        print_error_message(WRONG_ARG_NUMBER, _fd);
        return ;
    }

    for (size_t i = 1; i < msg.size(); i++)
        _username.push_back(msg[i]);
    user_set = true;
    std::cerr << "username entered" << std::endl; // bigboss test a suppr
}

// PASS / NICK / USER
void Client::identifying(std::string input, std::vector<Client *> clients)
{
    std::string cmd[3] = {"NICK", "USER"};
    std::vector<std::string> msg = splitString(input, ' ');

    size_t i = 0;
    while (i < 2 && cmd[i].compare(msg[0]))
        i++;

    switch(i)
    {
        case 0: // NICK
            set_nickname(msg, clients);
			break ;
        case 1: // USER
            set_usernames(msg);
			break ;
        default:
            print_error_message(COMMAND_NOT_FOUND, _fd);
    }
}

int Client::getFd() const
{
    return (_fd);
}

std::string Client::getName() const
{
    return (_name);
}

void Client::compareNames(std::string name)
{
    if (!_name.compare(name))
    {
        _name.clear();
        if (_username.size())
        {
            _name = _username[0];
            _username.erase(_username.begin());
        }
        return ;
    }

    for (size_t i = 0; i < _username.size(); i++)
        if (!_username[i].compare(name))
            _username.erase(_username.begin() + i);
}

void Client::addChannel(std::string name)
{
    std::string errorMessage;

    // check if already in the channel
    if (isInChannel(name))
    {
        errorMessage = "You already joined this channel\n";
        send(_fd, errorMessage.c_str(), errorMessage.length(), 0);
        return ;
    }

    // check if already in 10 channels (max)
    if (_channelNames.size() == MAX_CHANNEL)
    {
        errorMessage = "You can't join more channels\n";
        send(_fd, errorMessage.c_str(), errorMessage.length(), 0);
        return ;
    }

    _channelNames.push_back(name);
}

void Client::removeChannel(std::string name)
{
    std::vector<std::string>::iterator it;

    it = std::find(_channelNames.begin(), _channelNames.end(), name);
    if (it != _channelNames.end())
        _channelNames.erase(it);
}

bool Client::isInChannel(std::string name)
{
    std::vector<std::string>::iterator it;

    it = std::find(_channelNames.begin(), _channelNames.end(), name);
    if (it != _channelNames.end())
        return (true);
    return (false);
}

bool Client::hasInvite(std::string name)
{
    std::vector<std::string>::iterator it;

    it = std::find(_invites.begin(), _invites.end(), name);
    if (it != _invites.end())
        return (true);
    return (false);
}

void Client::saveInvite(std::string name)
{
    if (!hasInvite(name))
        _invites.push_back(name);
}

void Client::removeInvite(std::string name)
{
    std::vector<std::string>::iterator it;

    it = std::find(_invites.begin(), _invites.end(), name);
    if (it != _invites.end())
        _invites.erase(it);
}
