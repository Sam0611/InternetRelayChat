#ifndef __NUMERICREPLY__
#define __NUMERICREPLY__

#include "Server.hpp"

void    rpl_welcome(Client *client);
void    rpl_topic(Client *client, Channel *channel);
void    rpl_namereply(Client *client, Channel *channel);
void    rpl_endofnames(Client *client, Channel *channel);
void    rpl_inviting(Client *client1, Client *client2, std::string channel_name);
void    err_NEEDMOREPARAMS(int fd, std::string command);
void    err_passwdmismatch(int fd);


/*      DEFINE          */
#define RPL_WELCOME "001"
#define RPL_NOTOPIC "331"
#define RPL_TOPIC "332"
#define RPL_NAMREPLY "353"
#define RPL_ENDOFNAMES "366"
#define RPL_INVITING "341"
#define ERR_NEEDMOREPARAMS "461"
#define ERR_PASSWDMISMATCH "464"



#endif