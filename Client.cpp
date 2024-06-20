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

Client::Client() : info_set(false), _fd(-1), _pass(false)
{
}

Client::Client(int fd) : info_set(false), _fd(fd), _pass(false)
{
}

Client::~Client()
{
}

void Client::check_password_input(std::vector<std::string> msg, std::string password)
{
    if (_pass)
    {
        print_error_message(PARAMETER_ALREADY_SET, _fd);
        return ;
    }

    if (msg.size() != 2)
    {
        print_error_message(WRONG_ARG_NUMBER, _fd);
        return ;
    }

    if (!msg[1].compare(password))
        _pass = true;
    else
        print_error_message(WRONG_PASSWORD, _fd);
}

void Client::set_nickname(std::vector<std::string> msg)
{
    if (!_name.empty())
    {
        print_error_message(PARAMETER_ALREADY_SET, _fd);
        return ;
    }

    if (msg.size() != 2)
    {
        print_error_message(WRONG_ARG_NUMBER, _fd);
        return ;
    }

    for (size_t i = 0; i < _username.size(); i++)
    {
        if (!msg[1].compare(_username[i]))
        {
            std::cerr << RED << "Error: usernames must be different" << RESET << std::endl;
            return ;
        }
    }

    _name = msg[1];
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

    if (!_name.empty())
    {
        for (size_t i = 1; i < msg.size(); i++)
        {
            if (!msg[i].compare(_name))
            {
                std::cerr << RED << "Error: usernames must be different" << RESET << std::endl;
                return ;
            }
        }
    }

    for (size_t i = 1; i < msg.size(); i++)
    {
        for (size_t j = i + 1; j < msg.size(); j++)
        {
            if (!msg[i].compare(msg[j]))
            {
                std::cerr << RED << "Error: usernames must be different" << RESET << std::endl;
                return ;
            }
        }
    }

    for (size_t i = 1; i < msg.size(); i++)
        _username.push_back(msg[i]);
}

// PASS / NICK / USER
void Client::log_in(char *input, const std::string password)
{
	std::string str(input);
    str.erase(str.size() - 1); // remove \n at the end
    std::string cmd[3] = {"PASS", "NICK", "USER"};
    std::vector<std::string> msg = splitString(str, ' ');

    size_t i = 0;
    while (i < 3 && cmd[i].compare(msg[0]))
        i++;

    switch(i)
    {
        case 0: // PASS
            check_password_input(msg, password);
			break ;
        case 1: // NICK
            set_nickname(msg);
			break ;
        case 2: // USER
            set_usernames(msg);
			break ;
        default:
            print_error_message(COMMAND_NOT_FOUND, _fd);
    }
}

bool Client::check_informations()
{
    if (!_pass || _name.empty() || _username.size() != 4)
        info_set = false;
    else
        info_set = true;
    return (info_set);
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

void Client::saveInvite(std::string name)
{
    std::vector<std::string>::iterator it;

    it = std::find(_invites.begin(), _invites.end(), name);
    if (it == _invites.end())
        _invites.push_back(name);
}

void Client::removeInvite(std::string name)
{
    std::vector<std::string>::iterator it;

    it = std::find(_invites.begin(), _invites.end(), name);
    if (it != _invites.end())
        _invites.erase(it);
}
