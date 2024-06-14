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

Client::Client() : _fd(-1), _pass(false)
{
}

Client::Client(int fd) : _fd(fd), _pass(false)
{
}

Client::~Client()
{
}

void Client::check_password_input(std::vector<std::string> msg, std::string password)
{
    if (_pass)
    {
        std::cerr << RED << "Password already confirmed" << RESET << std::endl;
        return ;
    }

    if (msg.size() != 2)
    {
        std::cerr << RED << "Error: wrong number of arguments" << RESET << std::endl;
        return ;
    }

    if (!msg[1].compare(password))
        _pass = true;
    else
        std::cerr << RED << "Error: wrong password" << RESET << std::endl;
}

void Client::set_nickname(std::vector<std::string> msg)
{
    if (!_name.empty())
    {
        std::cerr << RED << "Nickname already set" << RESET << std::endl;
        return ;
    }

    if (msg.size() != 2)
    {
        std::cerr << RED << "Error: wrong number of arguments" << RESET << std::endl;
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
        std::cerr << RED << "Usernames already set" << RESET << std::endl;
        return ;
    }

    if (msg.size() != 5)
    {
        std::cerr << RED << "Error: wrong number of arguments" << RESET << std::endl;
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
    while (i < cmd->size() - 1 && cmd[i].compare(msg[0]))
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
            std::cerr << RED << "Wrong command, use PASS / NICK / USER" << RESET << std::endl;
    }
}

bool Client::check_informations() const
{
    if (!_pass || _name.empty() || _username.size() != 4)
        return (false);

    std::cout << _name << " (" << _username[0] << ", ";
    std::cout << _username[1] << ", ";
    std::cout << _username[2] << ", ";
    std::cout << _username[3] << ") is logged" << std::endl;

    return (true);
}
