/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcBot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbeaucie <sbeaucie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:23:23 by sbeaucie          #+#    #+#             */
/*   Updated: 2024/07/01 15:40:38 by sbeaucie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <csignal>
#include <cstdio>   // For popen, fgets, pclose

int		sockfd;
bool	isquit = false;

IrcBot::IrcBot() : server(""), port(0), password("") {
	sockfd = -1;
}

IrcBot::IrcBot(const std::string &server, int port, const std::string &password)
	: server(server), port(port), password(password) {
		sockfd = -1;
	}

IrcBot::IrcBot(const IrcBot &other) {
	*this = other;
}

IrcBot::~IrcBot()
{
	closeConnection();
}

void	IrcBot::closeConnection()
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

void	IrcBot::connect()
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

	std::string nickCmd = "NICK c2po\r\n";
	// identify connexion / data to send / size of data / flags
	send(sockfd, nickCmd.c_str(), nickCmd.size(), 0);

	// USER <username> <mode> <unused> :<realname>
	std::string usrCmd = "USER c2po 0 * :c2po\r\n";
	send(sockfd, usrCmd.c_str(), usrCmd.size(), 0);
}

void	quit(int signal)
{
	(void)signal;
	std::string quit = "QUIT\n";
	send(sockfd, quit.c_str(), quit.length(), 0);
	isquit = true;
}

void	IrcBot::run()
{
	char buffer[BUFFER_SIZE];
	while (true)
	{
		signal(SIGINT, quit);
		if (isquit)
			break ;
		int msgLen = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
		if (msgLen < 0)
		{
			std::cerr << "Error: recv() failed" << std::endl;
			break ;
		}
		buffer[msgLen] = '\0';
		std::string msg(buffer);
        std::cout << msg << std::endl;

		handleMessage(msg);
	}
}

void	IrcBot::sendMessage(const std::string &target, const std::string &msg)
{
	std::string message = "PRIVMSG " + target + " :" + msg + "\r\n";
	send(sockfd, message.c_str(), message.size(), 0);
}

void	IrcBot::handleCommand(const std::string &user, const std::string &cmd)
{
    std::string cleanedCmd = cmd;
    if (cleanedCmd.size() >= 2 && cleanedCmd.substr(cleanedCmd.size() - 2) == "\r\n") {
        cleanedCmd = cleanedCmd.substr(0, cleanedCmd.size() - 2);
    }

	if (cleanedCmd == "!hello") {
		sendMessage(user, "Bip Boup !");
	}
	else if (cleanedCmd == "!time")
	{
		time_t now = time(0);
		char *dt = ctime(&now);
		sendMessage(user, std::string("Current time: ") + dt);
	}
    else if (cleanedCmd == "!deepl help")
    {
        std::string languages = "Supported languages: DE, EN-GB, EN-US, BG, ZH, HR, DA, ES, ET, FI, FR, EL, HU, ID, IT, JA, LT, LV, NL, PL, PT-BR, PT-PT, RO, RU, SK, SL, SV, CS, TR, UK";
        sendMessage(user, languages);
    }
    else if (cleanedCmd.rfind("!deepl", 0) == 0)
    {
        std::vector<std::string> tokens = splitMessage(cleanedCmd, ' ');
        if (tokens.size() >= 3)
        {
            std::string targetLang = tokens[1];
            std::string text = cleanedCmd.substr(cleanedCmd.find(tokens[2]));
            std::string translatedText = translateText(text, targetLang);
            sendMessage(user, translatedText);
        }
        else
        {
            sendMessage(user, "Usage: !deepl <target_language> <text>");
        }
    }
}

void IrcBot::handleMessage(const std::string &msg)
{
	std::vector<std::string>	tokens = splitMessage(msg, ' ');
	if (tokens.size() > 3 && tokens[1] == "PRIVMSG")
	{
		std::string usr = tokens[0].substr(1);
		std::string cmd = tokens[3].substr(1); // remove ':'

		for (size_t i = 4; i < tokens.size(); ++i)
			cmd += " " + tokens[i];

		handleCommand(usr, cmd);
	}
}

void	IrcBot::joinChannel(const std::string &channel)
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
    The popen() (process open) function is used to execute a command in a child process and open a communication flow with that process.
    The fgets() function reads a line of characters from a stream and stores it as a string.
*/
std::string IrcBot::translateText(const std::string &text, const std::string &targetLang)
{
    std::string command = "python3 translate.py \"" + text + "\" " + targetLang;
    std::string result;
    char buffer[128];

    // Open a reading process by executing the command
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
		throw PipeErrorException();
	}
    try {
		// reads the process output line by line into the buffer.
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw TranslationException();
    }
    // Close process
    pclose(pipe);
    // Remove newline character from the end of result
    if (!result.empty() && result[result.size() - 1] == '\n') {
        result.erase(result.size() - 1);
    }
    return (result);
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
const char* IrcBot::PipeErrorException::what() const throw() {
    return ("Error: Pipe operation failed.");
}
const char* IrcBot::TranslationException::what() const throw() {
    return ("Error: Translation failed.");
}
