
#include "utils.hpp"

std::vector<std::string> splitString(std::string str, char delimiter)
{
    std::vector<std::string> vec;
    size_t i;

    do
    {
        i = str.find(delimiter);
        if (i != 0)
            vec.push_back(str.substr(0, i));
        str.erase(0, i);
        while (str[0] == delimiter)
            str.erase(0, 1);
    }
    while (!str.empty() && i != std::string::npos);

    return (vec);
}

std::vector<std::string> splitString(std::string str, char delimiter, char end)
{
    std::vector<std::string> vec;
    size_t i;

    do
    {
        i = str.find(delimiter);
        if (i != 0)
            vec.push_back(str.substr(0, i));
        str.erase(0, i);
        while (str[0] == delimiter)
            str.erase(0, 1);
    }
    while (!str.empty() && i != std::string::npos && str[0] != end);

    if (!str.empty())
        vec.push_back(str);

    return (vec);
}

void print_error_message(int e, int fd)
{
    std::string errorMessage = ":";
    errorMessage.append(SERVER);

    switch(e)
    {
        case PASSWORD_MISSMATCH:
            errorMessage.append(" 464 :Password incorrect\r\n");
            break ;
        case PASSWORD_EMPTY:
            errorMessage.append(" :Password can't be empty\r\n");
			break ;
        case NO_NICKNAME_GIVEN:
            errorMessage.append(" 431 :No nickname given\r\n");
            break ;
        case NICKNAME_FORBIDDEN:
            errorMessage.append(" 432 :Nickname contains forbidden character\r\n");
            break ;
        case NICKNAME_ALREADY_USE:
            errorMessage.append(" 433 :Nickname is already in use\r\n");
            break ;
        case NICKNAME_ALREADY_SET:
            errorMessage.append(" :Nickname is already set\r\n");
            break ;
        case ALREADY_REGISTERED:
            errorMessage.append(" 462 :Already registered\r\n");
            break ;
        case NOT_NICK_USER:
            errorMessage.append(" :Must enter Nick/User\r\n");
            break ;
        case COMMAND_NOT_FOUND:
            errorMessage.append(" :wrong command\r\n");
			break ;
        case NEED_MORE_PARAM:
            errorMessage.append(" 461 :Not enough paramaeters\r\n");
            break ;
        case WRONG_ARG_NUMBER:
            errorMessage.append(" :wrong number of arguments\r\n");
			break ;
        case USERNAME_NOT_AVAILABLE:
            errorMessage.append(" :username not available\r\n");
			break ;
        case WRONG_PASSWORD:
            errorMessage.append(" :wrong password\r\n");
			break ;
        case PARAMETER_ALREADY_SET:
            errorMessage.append(" :parameter already set\r\n");
			break ;
        case INVALID_NAME:
            errorMessage.append(" :invalid name\r\n");
			break ;
        case USER_NOT_FOUND:
            errorMessage.append(" :user not found\r\n");
			break ;
        case INVALID_FORMAT:
            errorMessage.append(" :invalid format\r\n");
			break ;
        case TOO_MANY_KEYS:
            errorMessage.append(" :there is more passwords than channel names\r\n");
			break ;
        case PERM_DENIED:
            errorMessage.append(" :permission denied\r\n");
			break ;
        case CHANNEL_NOT_FOUND:
            errorMessage.append(" :channel not found\r\n");
			break ;
        case INVALID_PASSWORD:
            errorMessage.append(" :invalid password\r\n");
			break ;
        case LIMIT_TOO_SMALL:
            errorMessage.append(" :limit is too small\r\n");
			break ;
        case LIMIT_EXCEEDED:
            errorMessage.append(" :limit must be greater then number of members\r\n");
			break ;
        case NOT_IN_THE_CHANEL:
            errorMessage.append(" :not in the chanel\r\n");
			break ;
        case NO_SUCH_NICK:
            errorMessage.append(" :no such nick/channel\r\n");
			break ;
        case CHANNEL_PASSWORD_INCORRECT:
            errorMessage.append(" :incorrect channel password\r\n");
			break ;
        default:
            std::cerr << RED << " :error 404" << RESET << std::endl;
            return ;
    }

    send(fd, errorMessage.c_str(), errorMessage.length(), 0);
    std::cerr << RED << errorMessage << RESET;
}