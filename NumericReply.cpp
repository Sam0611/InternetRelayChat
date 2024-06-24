#include "NumericReply.hpp"

void    rpl_welcome(Client client)
{
    //Welcome new user
    //model : ":server 001 <nick> :Welcome to the <network> Network, <nick>[!<user>@<host>]"
    //ex : ":irc.example.com 001 dan :Welcome to the IRCcom Network, dan"
    std::string message;

    message = ":";
    message.append(SERVER);
    message.append(" ");
    message.append(RPL_WELCOME);
    message.append(" ");
    message.append(client.getName());
    message.append(" :Welcome to the ");
    message.append(NETWORK);
    message.append(", ");
    message.append(client.getName());
    message.append("\n");
    send(client.getFd(), message.c_str(), message.length(), 0);
}

void    rpl_topic(Client client, Channel channel)
{
    //display Topic
    std::string message;

    message.append(SERVER);
    message.append(" ");
    if (channel.getTopic().empty())
        message.append(RPL_NOTOPIC);
    else
        message.append(RPL_TOPIC);
    message.append(" ");
    message.append(client.getName());
    message.append(" ");
    message.append(channel.getName());
    message.append(" ");

    if (channel.getTopic().empty())
        message.append(":No topic is set.");
    else
        message.append(channel.getTopic());
    message.push_back('\n');
    send(client.getFd(), message.c_str(), message.length(), 0);
}