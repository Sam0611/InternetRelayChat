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

bool Channel::isMember(std::string name)
{
    std::map<std::string, int>::iterator it = _users.find(name);
    if (it != _users.end())
        return (true);
    return (false);
}

void Channel::addUser(std::string name, int fd)
{
    // check if user is already in the channel
    if (isMember(name))
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
    // insert sender's and channel's name at the beginning of message
    message.insert(0, _name.substr(1));
    message.insert(0, "@");
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

bool Channel::isOperator(std::string name)
{
    for (size_t i = 0; i < _operator.size(); i++)
        if (!_operator[i].compare(name))
            return (true);
    return (false);
}

std::string Channel::getTopic() const
{
    return (_topic);
}

void Channel::setTopic(std::string name, std::string topic)
{
    _topic = topic;

    // send message to all members saying topic has changed
    std::string message = " changed topic to ";
    message.append(topic);
    message.append("\n");
    sendMessage(name, message);
}

bool Channel::checkPassword(std::string password) const
{
    if (!password.compare(_password))
        return (true);
    return (false);
}

bool Channel::limitReached() const
{
    if (_users.size() < _limit)
        return (false);
    return (true);
}

bool Channel::getMode(char mode) // i/t/k/l
{
    std::map<char, bool>::iterator it = _mode.find(mode);
    if (it != _mode.end())
        return (_mode[mode]);
    return (false);
}

// i / t / -k / -l
void Channel::changeMode(char change, char mode)
{
    if (change == '-')
        _mode[mode] = false;
    if (change == '+')
        _mode[mode] = true;
}

// +l
void Channel::changeMode(char mode, size_t len, int fd)
{
    if (len <= 0)
    {
        print_error_message(LIMIT_TOO_SMALL, fd);
        return ;
    }

    if (len < _users.size())
    {
        print_error_message(LIMIT_EXCEEDED, fd);
        return ;
    }

    _mode[mode] = true;
    _limit = len;
}

// o / +k
void Channel::changeMode(char change, char mode, std::string str)
{
    if (mode == 'k')
    {
        _mode[mode] = true;
        _password = str;
        return ;
    }

    // mode = o
    std::vector<std::string>::iterator it = std::find(_operator.begin(), _operator.end(), str);
    if (change == '-' && it != _operator.end())
        _operator.erase(it);
    if (change == '+' && it == _operator.end())
        _operator.push_back(str);
}