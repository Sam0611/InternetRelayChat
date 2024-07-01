/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcBot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbeaucie <sbeaucie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:23:30 by sbeaucie          #+#    #+#             */
/*   Updated: 2024/06/26 19:17:20 by sbeaucie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
# define IRCBOT_HPP

# include <string>
# include <vector>

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

    std::string translateText(const std::string &text, const std::string &targetLang);
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
    class PipeErrorException : public std::exception
    {
        const char *what() const throw();
    };
    class TranslationException : public std::exception
    {
        const char *what() const throw();
    };

};

#endif
