
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
    std::string errorMessage;

    switch(e)
    {
        case COMMAND_NOT_FOUND:
            errorMessage = "Error: wrong command\n";
			break ;
        case WRONG_ARG_NUMBER:
            errorMessage = "Error: wrong number of arguments\n";
			break ;
        case USERNAME_NOT_AVAILABLE:
            errorMessage = "Error: username not available\n";
			break ;
        case WRONG_PASSWORD:
            errorMessage = "Error: wrong password\n";
			break ;
        case PARAMETER_ALREADY_SET:
            errorMessage = "Error: parameter already set\n";
			break ;
        case INVALID_NAME:
            errorMessage = "Error: invalid name\n";
			break ;
        case USER_NOT_FOUND:
            errorMessage = "Error: user not found\n";
			break ;
        case INVALID_FORMAT:
            errorMessage = "Error: invalid format\n";
			break ;
        case TOO_MANY_KEYS:
            errorMessage = "Error: there is more passwords than channel names\n";
			break ;
        case PERM_DENIED:
            errorMessage = "Error: permission denied\n";
			break ;
        case CHANNEL_NOT_FOUND:
            errorMessage = "Error: channel not found\n";
			break ;
        default:
            std::cerr << RED << "error 404" << RESET << std::endl;
            return ;
    }

    send(fd, errorMessage.c_str(), errorMessage.length(), 0);
    std::cerr << RED << errorMessage << RESET;
}