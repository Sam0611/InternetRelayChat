/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 13:26:11 by sam               #+#    #+#             */
/*   Updated: 2024/06/13 13:26:13 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string name, std::string opName, int opFd) : _name(name)
{
    _mode.insert(std::pair<char, bool>('i', false));
    _mode.insert(std::pair<char, bool>('t', true));
    _mode.insert(std::pair<char, bool>('k', false));
    _mode.insert(std::pair<char, bool>('l', false));
    _operator.push_back(opName);
    _users.insert(std::pair<std::string, int>(opName, opFd));

    std::cout << opName << " created " << name << std::endl;
}

Channel::~Channel()
{
}

std::string Channel::getName() const
{
    return (_name);
}

void Channel::addUser(std::string name, int fd)
{
    // check if user is already in the channel
    std::map<std::string, int>::iterator it = _users.find(name);
    if (it != _users.end())
    {
        std::cout << name << " is already in the channel " << _name << std::endl;
        return ;
    }

    // add user to channel
    _users.insert(std::pair<std::string, int>(name, fd));

    // send join message to server and everyone in the channel
    std::cout << name << " joined " << _name << std::endl;
    std::string message = " joined ";
    message.append(_name);
    message.push_back('\n');
    sendMessage(name, message);
}

void Channel::removeUser(std::string name)
{
    std::map<std::string, int>::iterator it = _users.find(name);
    if (it != _users.end())
        _users.erase(name);
}

void Channel::sendMessage(std::string name, std::string message)
{
    // insert sender's name at the beginning of message
    message.insert(0, name);

    // send the message to every users in the channel
    std::map<std::string, int>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        // if it points on sender's name, pass
        if (it->first == name)
            continue ;

        send(it->second, message.c_str(), message.length(), 0);
    }
}

bool Channel::getMode(char mode)
{
    std::map<char, bool>::iterator it = _mode.find(mode);
    if (it != _mode.end())
        return (_mode[mode]);
    return (false);
}

void Channel::changeMode(char mode, char change)
{
    std::map<char, bool>::iterator it = _mode.find(mode);
    if (it == _mode.end())
        return ;

    if (change != '-' && change != '+')
        return ;

    // change mode
}
