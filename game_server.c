/**
 * game_server.c
 *
 * Main Implementation of the game server control logic. Not the game logic.
 */

#include "game_server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <stdbool.h>
#include <SDL.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <poll.h>

#include "utils.h"

static bool s_game_server_running = false;
static pthread_t s_game_server_thread;
static int s_game_server_receive_socket_v4;
static struct sockaddr_in s_server_sockaddr;

/**
 * \brief Game server remote connection information
 *
 * This structure keeps track of information related to remote connections
 * with clients connected to this game server.
 */
struct gs_remote_connection {
    struct sockaddr_in gsrc_sockaddr_info;
    int                gsrc_remote_fd; /** File descriptor to talk with the peer */
};

static struct gs_remote_connection *s_game_server_remote_conns = NULL;
static size_t s_game_server_num_remote_conns = 0;

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
    /* As per the man pages for getsockname, it really should not be possible
     * to have a failure here! */
    assert(rc == 0);
    SDL_Log("Socket name: %u %u %u",
            s_server_sockaddr.sin_family,
            s_server_sockaddr.sin_port,
            s_server_sockaddr.sin_addr.s_addr);
}

static void
game_server_network_close_open_connections (void)
{
    size_t i;
    for (i = 0; i < s_game_server_num_remote_conns; i++) {
        close(s_game_server_remote_conns[i].gsrc_remote_fd);
    }

    if (s_game_server_num_remote_conns > 0) {
        free(s_game_server_remote_conns);
        s_game_server_remote_conns = NULL;
        s_game_server_num_remote_conns = 0;
    }
}

static void
game_server_close_socket (void)
{
    game_server_network_close_open_connections();

    int rc = close(s_game_server_receive_socket_v4);
    if (rc != 0) {
        SDL_Log("Failed to close socket: %u", errno);
    }

    memset(&s_server_sockaddr, 0, sizeof(s_server_sockaddr));
}

static void
game_server_network_open_pending_connection (void)
{
    int new_connection_fd;
    struct sockaddr_in incoming_connection_addr;
    unsigned int out_sockaddr_len = sizeof(incoming_connection_addr);
    void *new_remote_conn_arr;

    SDL_Log("Handling pending connection!");

    new_connection_fd = accept(s_game_server_receive_socket_v4,
                               (struct sockaddr *)&incoming_connection_addr,
                               &out_sockaddr_len);
    if (new_connection_fd == -1) {
        SDL_Log("Failed to accept new connection: %u", errno);
        return;
    }

    if (out_sockaddr_len != sizeof(incoming_connection_addr)) {
        SDL_Log("Unexpected address info length %u, expected %lu",
                out_sockaddr_len, sizeof(incoming_connection_addr));
        exit(EXIT_FAILURE);
    }

    s_game_server_num_remote_conns++;
    new_remote_conn_arr = realloc(s_game_server_remote_conns,
                                  s_game_server_num_remote_conns *
                                  sizeof(struct gs_remote_connection));
    if (new_remote_conn_arr == NULL) {
        close(new_connection_fd);
        SDL_Log("Failed to make space for a new connection");
        exit(EXIT_FAILURE);
    }

    s_game_server_remote_conns =
        (struct gs_remote_connection *)new_remote_conn_arr;
    s_game_server_remote_conns[s_game_server_num_remote_conns-1].gsrc_remote_fd =
        new_connection_fd;
    s_game_server_remote_conns[s_game_server_num_remote_conns-1].gsrc_sockaddr_info =
        incoming_connection_addr;
}

static void
game_server_network_handle_pending_conns (void)
{
    int num_ready_fds = 0;

    struct pollfd receive_poll_info = {
        .fd = s_game_server_receive_socket_v4,
        .events = POLLRDNORM | POLLRDBAND,
        .revents = 0,
    };

    num_ready_fds = poll(&receive_poll_info, (nfds_t)1, 0);

    if (num_ready_fds == -1) {
        SDL_Log("Failed to poll when handling pending connections: %u",
                errno);
        return;
    }

    if (num_ready_fds == 1) {
        game_server_network_open_pending_connection();
    }
}

static void
game_server_network_handle_socket_read (int pending_fd)
{
    uint8_t read_buf[100] = {0};
    ssize_t num_bytes_read = 0;

    num_bytes_read = read(pending_fd, read_buf, sizeof(read_buf));
    if (num_bytes_read == -1) {
        SDL_Log("Failed to read from fd %u: %u", pending_fd, errno);
        return;
    }

    SDL_Log("Got data from socket! %s\n", (char *)read_buf);
}

static void
game_server_network_handle_pending_reads (void)
{
    int num_ready_fds = 0;
    size_t i;
    struct pollfd fds_to_poll[s_game_server_num_remote_conns];

    for (i = 0 ; i < s_game_server_num_remote_conns; i++) {
        fds_to_poll[i].fd = s_game_server_remote_conns[i].gsrc_remote_fd;
        fds_to_poll[i].events = POLLRDNORM;
        fds_to_poll[i].revents = 0;
    }

    num_ready_fds = poll((struct pollfd *)&fds_to_poll,
                         (nfds_t)s_game_server_num_remote_conns,
                         0);

    if (num_ready_fds == -1) {
        SDL_Log("Failed to select when handling pending reads: %u", errno);
        return;
    }

    for (i = 0; i < s_game_server_num_remote_conns; i++) {
        if ((fds_to_poll[i].revents & POLLRDNORM) != 0) {
            game_server_network_handle_socket_read(
                fds_to_poll[i].fd);
        }
    }
}

static void
game_server_handle_network (void)
{
    game_server_network_handle_pending_conns();
    game_server_network_handle_pending_reads();
}

static void *
game_server_thread (void *arg)
{
    bool printed = false;
    (void)pthread_setname_np("game_server");

    game_server_open_socket();

    while (s_game_server_running == true) {
        if (!printed) {
            printed = true;
            SDL_Log("Game server running");
        }

        game_server_handle_network();
    }

    game_server_close_socket();

    return NULL;
}

void
game_server_start (void)
{
    int rc;

    rc = pthread_create(&s_game_server_thread, NULL,
                        game_server_thread, NULL);
    if (rc == 0) {
        s_game_server_running = true;
    } else {
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
