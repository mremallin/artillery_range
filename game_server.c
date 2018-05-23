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

#include "utils.h"

static bool s_game_server_running = false;
static pthread_t s_game_server_thread;
static int s_game_server_receive_socket_v4;
static struct sockaddr_in s_server_sockaddr;

struct gs_remote_connection {
    struct sockaddr_in gsrc_sockaddr_info;
    int                gsrc_remote_fd;
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
game_server_network_close_open_connections (void)
{
    size_t i;
    for (i = 0; i < s_game_server_num_remote_conns; i++) {
        close(s_game_server_remote_conns[i].gsrc_remote_fd);
    }

    free(s_game_server_remote_conns);
    s_game_server_num_remote_conns = 0;
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

    s_game_server_remote_conns = new_remote_conn_arr;
    s_game_server_remote_conns[s_game_server_num_remote_conns-1].gsrc_remote_fd =
        new_connection_fd;
    s_game_server_remote_conns[s_game_server_num_remote_conns-1].gsrc_sockaddr_info =
        incoming_connection_addr;
}

static void
game_server_network_handle_pending_conns (void)
{
    int num_ready_fds = 0;
    struct timespec pselect_timeout = {
        .tv_sec = 0,
        .tv_nsec = MSEC_TO_NSEC(5),
    };

    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(s_game_server_receive_socket_v4, &read_fds);

    num_ready_fds = pselect(s_game_server_receive_socket_v4 + 1, &read_fds,
                            NULL /* Write FDs */,
                            NULL /* Exception FDs */,
                            &pselect_timeout,
                            NULL /* Signal mask */);
    if (num_ready_fds == -1) {
        SDL_Log("Failed to select when handling pending connections: %u",
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
    uint8_t read_buf[100];
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
    struct timespec pselect_timeout = {
        .tv_sec = 0,
        .tv_nsec = MSEC_TO_NSEC(1),
    };
    size_t i;
    int nfds = 0;

    fd_set read_fds;

    FD_ZERO(&read_fds);

    for (i = 0 ; i < s_game_server_num_remote_conns; i++) {
        FD_SET(s_game_server_remote_conns[i].gsrc_remote_fd,
               &read_fds);
        nfds = MAX(s_game_server_remote_conns[i].gsrc_remote_fd,
                   nfds);
    }

    num_ready_fds = pselect(nfds+1,
                            &read_fds,
                            NULL /* Write FDs */,
                            NULL /* Exception FDs */,
                            &pselect_timeout,
                            NULL /* Signal mask */);

    if (num_ready_fds == -1) {
        SDL_Log("Failed to select when handling pending reads: %u", errno);
        return;
    }

    for (i = 0; i < s_game_server_num_remote_conns; i++) {
        if (FD_ISSET(s_game_server_remote_conns[i].gsrc_remote_fd,
                     &read_fds)) {
            game_server_network_handle_socket_read(
                s_game_server_remote_conns[i].gsrc_remote_fd);
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
