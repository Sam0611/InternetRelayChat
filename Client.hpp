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

class Server;

class Client
{
    public:
        Client();
        Client(int fd);
        ~Client();
        void identifying(std::string input, std::vector<Client *> clients);
        bool pass_set;
        bool nick_set;
        bool user_set;
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
        void check_password_input(std::string input, std::string password);
        void set_nickname(std::vector<std::string> msg, std::vector<Client *> clients);
        void set_usernames(std::vector<std::string> msg);

    private:
        int         _fd;
        std::string _name;
        std::vector<std::string> _username;
        std::vector<std::string> _channelNames;
        std::vector<std::string> _invites;
};

#endif
