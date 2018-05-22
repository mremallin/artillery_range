#include "game_client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <SDL.h>
#include <netinet/ip.h>

static bool s_game_client_running = false;
static pthread_t s_game_client_thread;
static struct sockaddr_in s_client_sockaddr;

static void*
game_client_thread (void *arg)
{
    bool printed = false;

    while (s_game_client_running) {
        if (!printed) {
            printed = true;
            SDL_Log("Game client running - Will attempt connection on port %u",
                    s_client_sockaddr.sin_port);
        }
    }

    return NULL;
}

void
game_client_start (struct sockaddr_in *connect_info)
{
    int rc;

    s_game_client_running = true;
    memcpy(&s_client_sockaddr, connect_info, sizeof(*connect_info));

    rc = pthread_create(&s_game_client_thread, NULL,
                        game_client_thread, NULL);
    if (rc != 0) {
        SDL_Log("Failed to create game client thread: %u", rc);
        exit(EXIT_FAILURE);
    }
}

void
game_client_finish (void)
{
    int rc;

    s_game_client_running = false;

    rc = pthread_join(s_game_client_thread, NULL);
    if (rc != 0) {
        SDL_Log("Failed to join game client thread: %u", rc);
    }

    memset(&s_client_sockaddr, 0, sizeof(s_client_sockaddr));
}
