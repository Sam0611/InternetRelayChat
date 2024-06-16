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

Channel::Channel(std::string name, std::string op) : _name(name)
{
    _mode.insert(std::pair<char, bool>('i', false));
    _mode.insert(std::pair<char, bool>('t', true));
    _mode.insert(std::pair<char, bool>('k', false));
    _mode.insert(std::pair<char, bool>('l', false));
    _operator.push_back(op);
}

Channel::~Channel()
{
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
