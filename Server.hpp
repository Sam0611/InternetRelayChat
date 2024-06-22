/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 18:04:59 by sam               #+#    #+#             */
/*   Updated: 2024/06/09 18:05:03 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER__
#define __SERVER__

#include "utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server
{
    private:
        Server();
        std::string             _host;
        const std::string       _password;
        long                    _port;
        int                     _socket;
        std::vector<Client *>   _clients;
        std::vector<Channel *>   _channels;
        int getFdByName(const std::string name);
        size_t getClientId(const std::string name);
        size_t getChannelId(const std::string name);
        void process_commands(char *input, int id);
        void send_private_message(std::vector<std::string> msg, int id);
        void join_channel(std::vector<std::string> msg, int id);
        void leave_channel(std::vector<std::string> msg, int id);
        void view_or_change_topic(std::vector<std::string> msg, int id);
        void invite_to_channel(std::vector<std::string> msg, int id);
        void kick_from_channel(std::vector<std::string> msg, int id);
        void change_mode(std::vector<std::string> msg, int id);
        void list_channels(std::vector<std::string> msg, int id);

    public:
        Server(std::string password);
        ~Server();
        int createServer(char *input);
        int startServer(void);
};

#endif
