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
    int         sockfd;

    void    handleMessage(const std::string &msg);
    std::vector<std::string>    splitMessage(const std::string &msg, char delimiter);
    void closeConnection();
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
    
};

#endif
