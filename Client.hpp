/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 13:26:28 by sam               #+#    #+#             */
/*   Updated: 2024/06/13 13:26:29 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT__
#define __CLIENT__

#include "utils.hpp"

class Client
{
    public:
        Client();
        Client(int fd);
        ~Client();
        void log_in(char *input, const std::string password);
        bool info_set;
        bool check_informations();
        int getFd() const;
        std::string getName() const;
        void compareNames(std::string name);
        void addChannel(std::string name);
        void removeChannel(std::string name);
        bool isInChannel(std::string name);
        bool hasInvite(std::string name);
        void saveInvite(std::string name);
        void removeInvite(std::string name);

    private:
        int         _fd;
        bool        _pass;
        std::string _name;
        std::vector<std::string> _username;
        std::vector<std::string> _channelNames;
        std::vector<std::string> _invites;
        void check_password_input(std::vector<std::string> msg, std::string password);
        void set_nickname(std::vector<std::string> msg);
        void set_usernames(std::vector<std::string> msg);
};

#endif
