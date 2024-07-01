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
#include "Client.hpp"

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

    // format join notif message
    std::string message = ":";
    message.append(name);
    message.append(" JOIN ");
    message.append(_name);
    message.append("\r\n");

    // send join message to server and everyone in the channel
    std::cout << ":" << name << " JOIN " << _name << std::endl;
    std::map<std::string, int>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        send(it->second, message.c_str(), message.length(), 0);
    }
}

void Channel::removeUser(std::string name)
{
    std::map<std::string, int>::iterator it = _users.find(name);
    if (it != _users.end())
        _users.erase(name);
}

void Channel::removeOperator(std::string name)
{
    for (size_t i = 0; i < _operator.size(); i++)
    {
        if (name == _operator[i])
        {
            _operator[i].erase();
            break ;
        }
    }
}

void Channel::sendMessage(std::string name, std::string content)
{
    // format message channel
    std::string message = ":";
    message.append(name);
    message.append(" PRIVMSG ");
    message.append(_name);
    message.append(" ");
    message.append(content);

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

void Channel::joinChannelMessage(int fd, std::string name)
{
    // entered message
    std::string message = ":";
    message.append(name);
    message.append(" JOIN ");
    message.append(_name);
    message.append("\r\n");
    send(fd, message.c_str(), message.length(), 0);
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
    message.append("\r\n");
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

bool Channel::needArgMode(bool activate, char mode)
{
    if (activate == true && (mode == 'k' || mode == 'l' || mode == 'o'))
        return (true);
    return (false);
}

// i / t / -k / -l
void Channel::changeMode(bool activate, char mode)
{
    std::string message = ":";
    message.append(SERVER);
    message.append(" MODE ");
    message.append(_name);
    if (!activate)
    {
        _mode[mode] = false;
        message.append(" -");
    }
    else if (activate)
    {
        _mode[mode] = true;
        message.append(" +");
    }
    message.push_back(mode);
    message.append("\r\n");
    sendMessageloop(message);
}

// +l
void Channel::changeMode(char mode, size_t len, Client *client, std::string len_str)
{
    if (len <= 0)
    {
        print_error_message(LIMIT_TOO_SMALL, client->getFd());
        return ;
    }

    if (len < _users.size())
    {
        print_error_message(LIMIT_EXCEEDED, client->getFd());
        return ;
    }

    _mode[mode] = true;
    _limit = len;

    // server response
    std::string message = ":";
    message.append(client->getName());
    message.append(" MODE ");
    message.append(_name);
    message.append(" +l ");
    message.append(len_str);
    message.append("\r\n");
    sendMessageloop(message);
}

// +k
void Channel::changeMode(bool activate, char mode, std::string pass)
{
    if (pass.empty() || _password == pass)
        return ;
    _mode[mode] = true;
    _password = pass;

    // server response
    std::string message = ":";
    message.append(SERVER);
    message.append(" MODE ");
    message.append(_name);
    message.append(" ");
    if (activate)
        message.append("+");
    else
        message.append("-");
    message.push_back(mode);
    message.append(" ");
    message.append(pass);
    message.append("\r\n");
    sendMessageloop(message);
}

// o
void Channel::changeOperator(bool activate, std::string name)
{
    std::vector<std::string>::iterator it = std::find(_operator.begin(), _operator.end(), name);
    std::string message = ":";
    message.append(SERVER);
    message.append(" MODE ");
    message.append(_name);
    if (!activate && it != _operator.end())
    {
        _operator.erase(it);
        message.append(" -o ");
    }
    else if (activate && it == _operator.end())
    {
        _operator.push_back(name);
        message.append(" +o ");
    }
    else
        return;
    message.append(name);
    message.append("\r\n");
    sendMessageloop(message);
}

void Channel::sendMessageloop(std::string message)
{
    std::map<std::string, int>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        send(it->second, message.c_str(), message.length(), 0);
    }
}

void Channel::sendMessageloopExcept(std::string message, std::string except)
{
    std::map<std::string, int>::iterator it;
    for (it = _users.begin(); it != _users.end(); it++)
    {
        if (it->first != except)
            send(it->second, message.c_str(), message.length(), 0);
    }
}

std::map<std::string, int>::iterator Channel::getUsersbegin(void)
{
    return (_users.begin());
}

std::map<std::string, int>::iterator Channel::getUsersend(void)
{
    return (_users.end());
}
bool Channel::getChannelSize(void)
{
    return (_users.size());
}
