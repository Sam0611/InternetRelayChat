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
        void process_commands(char *input, int id);
        void send_private_message(std::vector<std::string> msg, int id);

    public:
        Server(std::string password);
        ~Server();
        int createServer(char *input);
        int startServer(void);
};

#endif
