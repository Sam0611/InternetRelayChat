#include "IrcBot.hpp"
#include <iostream>
#include <cstdlib>

int main(int ac, char **av)
{
    if (ac != 4)
    {
        std::cerr << "Usage: " << av[0] << " <server> <port> <password>" << std::endl;
        return (1);
    }

    try {
        std::string server = av[1];
        int         port = atoi(av[2]);
        std::string password = av[3];

        IrcBot bot(server, port, password);
        bot.connect();
        bot.joinChannel("#r2d3");
        bot.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return (0);
}