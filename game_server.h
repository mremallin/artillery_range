#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__
#include <netinet/ip.h>

struct sockaddr_in*
game_server_get_running_bind_info();

void
game_server_start(void);

void
game_server_finish(void);

#endif /* __GAME_SERVER_H__ */
