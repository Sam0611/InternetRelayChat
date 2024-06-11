/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sam </var/spool/mail/sam>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 21:45:16 by sam               #+#    #+#             */
/*   Updated: 2024/06/09 21:45:17 by sam              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __UTILS__
#define __UTILS__

/*      INCLUDES        */

#include <iostream>
#include <string.h>
#include <unistd.h> // close
#include <stdlib.h> // strtol
#include <cctype> // isdigit
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in htons
#include <poll.h> // poll



/*      DEFINE          */

#define RED "\033[31;01m"
#define RESET "\033[00m"
#define ERROR -1
#define MAX_CONNEXION 10
#define BUFFER_SIZE 1024

#endif
