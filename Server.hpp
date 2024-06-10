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

class Server
{
    public:
        Server();
        ~Server();
        int createServer(char *input);
        int startServer(void);

    private:
        std::string _host;
        long        _port;
        int         _socket;
};

#endif
