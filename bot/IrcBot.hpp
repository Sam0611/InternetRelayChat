/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcBot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbeaucie <sbeaucie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:23:30 by sbeaucie          #+#    #+#             */
/*   Updated: 2024/07/01 17:28:16 by sbeaucie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
# define IRCBOT_HPP

# include <string>
# include <vector>

#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"
#define TITLE BOLD UNDERLINE

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define PINK "\033[0;38;5;199m"
#define RED_W "\033[41;37m"
#define YELLOW_W "\033[43;37m"
#define ERROR BOLD RED_W BLINK

# define BUFFER_SIZE 512

class IrcBot
{
private:
    std::string server;
    int         port;
    std::string password;

    void    handleMessage(const std::string &msg);
    std::vector<std::string>    splitMessage(const std::string &msg, char delimiter);
    void closeConnection();

    std::string translateText(const std::string &rawText, const std::string &rawTargetLang);
public:
    IrcBot();
    IrcBot(const std::string &server, int port, const std::string &password);
    IrcBot(const IrcBot &other);
    ~IrcBot();

    IrcBot& operator=(const IrcBot &other);

    void    connect();
    void    run();
    void    joinChannel(const std::string &channel);
    void    sendMessage(const std::string &target, const std::string &msg);
    void    handleCommand(const std::string &user, const std::string &cmd);

    class SocketErrorException : public std::exception {
        const char *what() const throw();
    };
    class AdressConversionExeption : public std::exception {
        const char *what() const throw();
    };
    class ConnectionFailedException : public std::exception {
        const char *what() const throw();
    };
    class TranslationException : public std::exception
    {
        const char *what() const throw();
    };

};

#endif
