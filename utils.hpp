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
#include <string>
#include <vector>
#include <map>
#include <sstream> // istringstream
#include <unistd.h> // close
#include <stdlib.h> // strtol
#include <cctype> // isdigit
#include <bits/stdc++.h> // std::find
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in htons recv
#include <poll.h> // poll
#include <cstdlib> //strtol
#include <csignal> //signal


/*      DEFINE          */

#define RED "\033[31;01m"
#define RESET "\033[00m"
#define ERROR -1
#define MAX_CONNEXION 100
#define MAX_CHANNEL 10
#define BUFFER_SIZE 512
#define FIRST_CLIENT 2
#define SERVER "スマブラチム"
#define MODES "iklot"


/*      ENUM            */

enum err
{
    PASSWORD_MISSMATCH,
    NICKNAME_ALREADY_SET,
    NICKNAME_ALREADY_USE,
    NICKNAME_FORBIDDEN,
    NO_NICKNAME_GIVEN,
    ALREADY_REGISTERED,
    COMMAND_NOT_FOUND,
    NEED_MORE_PARAM,
    WRONG_ARG_NUMBER,
    USERNAME_NOT_AVAILABLE,
    WRONG_PASSWORD,
    PARAMETER_ALREADY_SET,
    INVALID_NAME,
    USER_NOT_FOUND,
    INVALID_FORMAT,
    TOO_MANY_KEYS,
    PERM_DENIED,
    CHANNEL_NOT_FOUND,
    INVALID_PASSWORD,
    LIMIT_TOO_SMALL,
    LIMIT_EXCEEDED,
    NOT_IN_THE_CHANEL,
    NO_SUCH_NICK,
    CHANNEL_PASSWORD_INCORRECT
};

/*      FUNCTIONS       */

void print_error_message(int e, int fd);
std::vector<std::string> splitString(std::string str, char delimiter);
std::vector<std::string> splitString(std::string str, char delimiter, char end);

#endif
