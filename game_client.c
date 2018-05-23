#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

static bool s_game_client_running = false;
static pthread_t s_game_client_thread;
static struct sockaddr_in s_client_sockaddr;
static int s_game_client_server_socket_v4;

static void
game_client_open_socket (void)
{
    int rc;
    socklen_t s_client_sockaddr_size = sizeof(s_client_sockaddr);
    const struct sockaddr *connect_address = (const struct sockaddr *)&s_client_sockaddr;

    s_game_client_server_socket_v4 =
        socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_game_client_server_socket_v4 < 0) {
        SDL_Log("Failed to open client socket: %u", errno);
        exit(EXIT_FAILURE);
    }

    (void)inet_aton("127.0.0.1", (struct in_addr *)&s_client_sockaddr.sin_addr.s_addr);

    rc = connect(s_game_client_server_socket_v4, connect_address,
                 s_client_sockaddr_size);
    if (rc) {
        SDL_Log("Failed to connect: %u", errno);
        exit(EXIT_FAILURE);
    }

    SDL_Log("Successfully connected to server on port %u",
            s_client_sockaddr.sin_port);
}

static void
game_client_close_socket (void)
{
    int rc = close(s_game_client_server_socket_v4);
    if (rc) {
        SDL_Log("Failed to close socket: %u", errno);
    }
}

static void*
game_client_thread (void *arg)
{
    bool printed = false;
    int bytes_written = 0;

    game_client_open_socket();

    while (s_game_client_running == true) {
        if (!printed) {
            printed = true;
            bytes_written = write(s_game_client_server_socket_v4,
                                  "Test message!\n",
                                  sizeof("Test message!\n"));
            SDL_Log("Wrote %u bytes to socket", bytes_written);
        }
    }

    game_client_close_socket();

    return NULL;
}

void
game_client_start (struct sockaddr_in *connect_info)
{
    int rc;

    s_game_client_running = true;
    memcpy(&s_client_sockaddr, connect_info, sizeof(s_client_sockaddr));

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
