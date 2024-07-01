#include "NumericReply.hpp"

void    rpl_welcome(Client *client)
{
    // Welcome new user
    std::string message;

    message = ":";
    message.append(SERVER);
    message.append(" ");
    message.append(RPL_WELCOME);
    message.append(" ");
    message.append(client->getName());
    message.append(" :Welcome to the ");
    message.append(SERVER);
    message.append(", ");
    message.append(client->getName());
    message.append("\r\n");
    send(client->getFd(), message.c_str(), message.length(), 0);
}

void    rpl_topic(Client *client, Channel *channel)
{
    // display Topic
    std::string message;

    message = ":";
    message.append(SERVER);
    message.append(" ");
    if (channel->getTopic().empty())
        message.append(RPL_NOTOPIC);
    else
        message.append(RPL_TOPIC);
    message.append(" ");
    message.append(client->getName());
    message.append(" ");
    message.append(channel->getName());
    message.append(" ");

    if (channel->getTopic().empty())
        message.append(":No topic is set.");
    else
        message.append(channel->getTopic());
    message.push_back('\n');
    send(client->getFd(), message.c_str(), message.length(), 0);
}

void    rpl_namereply(Client *client, Channel *channel)
{
    // members message
    std::string message;

    message = ":";
    message.append(SERVER);
    message.append(" ");
    message.append(RPL_NAMREPLY);
    message.append(" ");
    message.append(client->getName());
    message.append(" = ");
    message.append(channel->getName());
    message.append(" :");

    for (std::map<std::string, int>::iterator it = channel->getUsersbegin(); it != channel->getUsersend(); it++)
    {
        if (channel->isOperator(it->first))
            message.append("@");
        message.append(it->first);
        message.append(" ");
    }
    message.push_back('\n');
    send(client->getFd(), message.c_str(), message.length(), 0);
}

void    rpl_endofnames(Client *client, Channel *channel)
{
    std::string message;

    message = ":";
    message.append(SERVER);
    message.append(" ");
    message.append(RPL_ENDOFNAMES);
    message.append(" ");
    message.append(client->getName());
    message.append(" ");
    message.append(channel->getName());
    message.append(" : End of /NAMES list\n");
    send(client->getFd(), message.c_str(), message.length(), 0);
}

void    rpl_inviting(Client *client1, Client *client2, std::string channel_name)
{
    std::string message;

    message = ":";
    message.append(SERVER);
    message.append(" ");
    message.append(RPL_INVITING);
    message.append(" ");
    message.append(client1->getName());
    message.append(" ");
    message.append(client2->getName());
    message.append(" ");
    message.append(channel_name);
    message.append("\r\n");
    send(client1->getFd(), message.c_str(), message.length(), 0);
}

void    err_needmoreparams(int fd, std::string command)
{
    std::string message = " ";
    message.append(SERVER);
    message.append(" ");
    message.append(ERR_NEEDMOREPARAMS);
    if (!command.empty())
    {
        message.append(" ");
        message.append(command);
    }
    message.append(" :Password Incorrect\r\n");
    send(fd, message.c_str(), message.length(), 0);
}

void    err_passwdmismatch(int fd)
{
    std::string message = " ";
    message.append(SERVER);
    message.append(" ");
    message.append(ERR_PASSWDMISMATCH);
    message.append(" :Password Incorrect\r\n");
    send(fd, message.c_str(), message.length(), 0);
}