/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sbeaucie <sbeaucie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 17:23:34 by sbeaucie          #+#    #+#             */
/*   Updated: 2024/06/26 17:24:00 by sbeaucie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        bot.joinChannel("#c2po");
        bot.run();
    } catch (const std::exception &e) {
        std::cerr << ERROR << e.what() << RESET << std::endl;
    }

    return (0);
}
