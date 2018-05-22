#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdbool.h>
#include <SDL.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/ip.h>

static bool s_game_server_running = false;
static pthread_t s_game_server_thread;
static int s_game_server_receive_socket_v4;
static struct sockaddr_in s_server_sockaddr;

struct sockaddr_in*
game_server_get_running_bind_info()
{
    return &s_server_sockaddr;
}

static void
game_server_open_socket (void)
{
    int rc;
    const struct sockaddr_in bind_address = {
        .sin_family = AF_INET,
        .sin_port = 0,
        .sin_addr = INADDR_ANY,
    };

    socklen_t s_server_sockaddr_size = sizeof(s_server_sockaddr);

    s_game_server_receive_socket_v4 =
        socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_game_server_receive_socket_v4 < 0) {
        SDL_Log("Failed to open socket: %u", errno);
        exit(EXIT_FAILURE);
    }

    rc = bind(s_game_server_receive_socket_v4,
              (const struct sockaddr *)&bind_address,
              sizeof(bind_address));
    if (rc == -1) {
        SDL_Log("Failed to bind socket: %u", rc);
        (void)close(s_game_server_receive_socket_v4);
        exit(EXIT_FAILURE);
    }

    rc = listen(s_game_server_receive_socket_v4,
                3 /* Connection backlog */);
    if (rc == -1) {
        SDL_Log("Failed to listen on socket: %u", rc);
        (void)close(s_game_server_receive_socket_v4);
        exit(EXIT_FAILURE);
    }

    rc = getsockname(s_game_server_receive_socket_v4,
                     (struct sockaddr *)&s_server_sockaddr,
                     &s_server_sockaddr_size);
    if (rc == -1) {
        SDL_Log("Failed to get socket name: %u", errno);
    } else {
        SDL_Log("Socket name: %u %u %u",
                s_server_sockaddr.sin_family,
                s_server_sockaddr.sin_port,
                s_server_sockaddr.sin_addr.s_addr);
    }
}

static void
game_server_close_socket (void)
{
    int rc = close(s_game_server_receive_socket_v4);
    if (rc != 0) {
        SDL_Log("Failed to close socket: %u", errno);
    }

    memset(&s_server_sockaddr, 0, sizeof(s_server_sockaddr));
}

static void *
game_server_thread (void *arg)
{
    bool printed = false;

    game_server_open_socket();

    while (s_game_server_running) {
        if (!printed) {
            printed = true;
            SDL_Log("Game server running");
        }
        /* TODO: Accept connections until game is started? */
    }

    game_server_close_socket();

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