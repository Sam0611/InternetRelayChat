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

    private:
        int _fd;
        std::string _username[4]; // maybe 5
        // std::vector<Channel *> _channels;
};

#endif
