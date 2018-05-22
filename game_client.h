#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__
#include <netinet/ip.h>

/* Takes the server information to bind to */
void
game_client_start(struct sockaddr_in*);

void
game_client_finish(void);

#endif /* __GAME_CLIENT_H__ */
