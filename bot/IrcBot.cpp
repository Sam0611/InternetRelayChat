#include "IrcBot.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h> // socket
#include <arpa/inet.h> // htons, sockaddr, inet_pton..
#include <ctime>
#include <string>
#include <sstream> // stringstream
#include <cstdlib> // exit
#include <unistd.h> // close

IrcBot::IrcBot() : server(""), port(0), password(""), sockfd(-1) {}

IrcBot::IrcBot(const std::string &server, int port, const std::string &password)
    : server(server), port(port), password(password), sockfd(-1) {}

IrcBot::IrcBot(const IrcBot &other) {
    *this = other;
}

IrcBot::~IrcBot()
{
    closeConnection();
}

void    IrcBot::closeConnection()
{
    if (sockfd != -1)
    {
        close(sockfd);
        sockfd = -1;
    }
}

IrcBot& IrcBot::operator=(const IrcBot& other) {
    if (this != &other) {
        server = other.server;
        port = other.port;
        password = other.password;
        sockfd = -1;
    }
    return (*this);
}

void    IrcBot::connect()
{
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
        throw SocketErrorException();
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; // adress family: IPv4
    serv_addr.sin_port = htons(port); // Port number, htons convert to the standard network byte order.

    // Converts server IP address from text to binary format.
    if (inet_pton(AF_INET, server.c_str(), &serv_addr.sin_addr) <= 0)
    {
        throw AdressConversionExeption();
    }
    if (::connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        throw ConnectionFailedException();
    }

    // Sends authentication commands to the IRC server.
    std::string passCmd = "PASS " + password + "\r\n";
    send(sockfd, passCmd.c_str(), passCmd.size(), 0);

    std::string nickCmd = "NICK R3D2\r\n";
    // identify connexion / data to send / size of data / flags
    send(sockfd, nickCmd.c_str(), nickCmd.size(), 0);

    // USER <username> <mode> <unused> :<realname>
    std::string usrCmd = "USER R3D2 0 * :R3D2\r\n";
    send(sockfd, usrCmd.c_str(), usrCmd.size(), 0);
}

void    IrcBot::run()
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        int msgLen = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (msgLen < 0)
        {
            std::cerr << "Error in recv()" << std::endl;
            break ;
        }
        buffer[msgLen] = '\0';
        std::string msg(buffer);
        handleMessage(msg);
    }
}

void    IrcBot::sendMessage(const std::string &target, const std::string &msg)
{
    std::string message = "PRIVMSG " + target + " :" + msg + "\r\n";
    send(sockfd, message.c_str(), message.size(), 0);
}

void    IrcBot::handleCommand(const std::string &user, const std::string &cmd)
{
    if (cmd == "!hello") {
        sendMessage(user, "Bip Boup !");
    }
    else if (cmd == "!time") 
    {
        time_t now = time(0);
        char *dt = ctime(&now);
        sendMessage(user, std::string("Current time: ") + dt);
    }
}

void IrcBot::handleMessage(const std::string &msg)
{
    std::vector<std::string>    tokens = splitMessage(msg, ' ');
    if (tokens.size() > 3 && tokens[1] == "PRIVMSG")
    {
        std::string usr = tokens[0].substr(1, tokens[0].find('!') - 1);
        std::string cmd = tokens[3].substr(1); // remove ':'

        for (size_t i = 4; i < tokens.size(); ++i)
            cmd += " " + tokens[i];

        handleCommand(usr, cmd);
    }
}

void    IrcBot::joinChannel(const std::string &channel)
{
    std::string joinCmd = "JOIN " + channel + "\r\n";
    send(sockfd, joinCmd.c_str(), joinCmd.size(), 0);
}

std::vector<std::string> IrcBot::splitMessage(const std::string &message, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream iss(message);
    std::string token;

    while (getline(iss, token, delimiter))
        tokens.push_back(token);
    
    return (tokens);
}

/*
    Exceptions
*/

const char* IrcBot::SocketErrorException::what() const throw() {
	return ("Error: Couldn't initialize socket.");
}
const char* IrcBot::AdressConversionExeption::what() const throw() {
	return ("Error: Invalid address.");
}
const char* IrcBot::ConnectionFailedException::what() const throw() {
	return ("Error: Failed to connect to the server.");
}