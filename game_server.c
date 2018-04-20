#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <SDL.h>
#include <assert.h>

static bool s_game_server_running = false;
static pthread_t s_game_server_thread;

static void *
game_server_thread (void *arg)
{
    bool printed = false;
    while (s_game_server_running) {
        if (!printed) {
            printed = true;
            SDL_Log("Game server running");
        }
    }

    return NULL;
}

void
game_server_start (void)
{
    int rc;

    s_game_server_running = true;

    rc = pthread_create(&s_game_server_thread, NULL,
                        game_server_thread, NULL);
    if (rc != 0) {
        SDL_Log("Failed to create game server thread: %u", rc);
        exit(EXIT_FAILURE);
    }
}

void
game_server_finish (void)
{
    int rc;

    s_game_server_running = false;

    rc = pthread_join(s_game_server_thread, NULL);
    if (rc != 0) {
        SDL_Log("Failed to join game server thread: %u", rc);
    }
}
