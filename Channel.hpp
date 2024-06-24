/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/13 13:26:19 by sam               #+#    #+#             */
/*   Updated: 2024/06/13 13:26:21 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANNEL__
#define __CHANNEL__

#include "utils.hpp"

class Channel
{
    private:
        Channel();
        std::string _name;
        std::map<char, bool> _mode; // char = i, t, k, l
        std::string _password;
        std::string _topic;
        size_t _limit;
        std::vector<std::string> _operator;
        std::map<std::string, int> _users;

    public:
        Channel(std::string name, std::string opName, int opFd);
        ~Channel();
        std::string getName() const;
        bool isMember(std::string name);
        void addUser(std::string name, int fd);
        void removeUser(std::string name);
        void sendMessage(std::string name, std::string message);
        void joinChannelMessage(int fd, std::string name);
        bool isOperator(std::string name);
        std::string getTopic() const;
        void setTopic(std::string name, std::string topic);
        bool checkPassword(std::string password) const;
        bool limitReached() const;
        bool getMode(char mode); // mode = i/t/k/o/l
        void changeMode(char change, char mode);
        void changeMode(char mode, size_t len, int fd);
        void changeMode(char change, char mode, std::string str);
};

#endif
