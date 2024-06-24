#ifndef __NUMERICREPLY__
#define __NUMERICREPLY__

#include "Server.hpp"

void    rpl_welcome(Client client);
void    rpl_topic(Client client, Channel channel);


/*      DEFINE          */

#define RPL_WELCOME "001"
#define RPL_NOTOPIC "331"
#define RPL_TOPIC "332"


#endif