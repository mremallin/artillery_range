#include "CppUTestExt/MockSupport.h"

#include <stdarg.h>
#include <exception>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

extern "C" {
    void
    SDL_Log(const char *fmt, ...)
    {
        va_list args;

        va_start(args, fmt);
        va_end(args);
    }

    int
    pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg)
    {
        int rc;
        mock().actualCall("pthread_create")
            .withParameter("thread", thread)
            .withOutputParameter("rc", &rc);
        return rc;
    }

    int
    pthread_join(pthread_t thread, void **value_ptr)
    {
        int rc;
        mock().actualCall("pthread_join")
            .withParameter("thread", thread)
            .withOutputParameter("rc", &rc);
        return rc;
    }

    void
    exit (int rc)
    {
        mock().actualCall("exit")
            .withParameter("rc", rc);
        throw std::exception();
    }

    int
    socket (int domain, int type, int protocol)
    {
        int rc;
        mock().actualCall("socket")
            .withParameter("domain", domain)
            .withParameter("type", type)
            .withParameter("protocol", protocol)
            .withOutputParameter("rc", &rc);
        return rc;
    }

    int
    connect(int socket, const struct sockaddr *address, socklen_t address_len)
    {
        int rc;
        struct sockaddr_in *sock_addr = (struct sockaddr_in *)address;
        mock().actualCall("connect")
            .withParameter("socket", socket)
            .withOutputParameter("rc", &rc);
        /* Currently only checking 127.0.0.1 */
        CHECK_EQUAL(sock_addr->sin_addr.s_addr, 0x0100007f);
        return rc;
    }

    int
    close (int fd)
    {
        int rc;
        mock().actualCall("close")
            .withParameter("fd", fd)
            .withOutputParameter("rc", &rc);
        return rc;
    }

    ssize_t
    write (int fildes, const void *buf, size_t nbyte)
    {
        ssize_t rc;
        mock().actualCall("write")
            .withParameter("fd", fildes)
            .withOutputParameter("rc", &rc);
        return rc;
    }

    int
    bind(int socket, const struct sockaddr *address, socklen_t address_len)
    {
        int rc;
        mock().actualCall("bind")
            .withParameter("socket", socket)
            .withOutputParameter("rc", &rc);
        return rc;
    }

    int
    getsockname(int socket, struct sockaddr *address,
                socklen_t *address_len)
    {
        return 0;
    }

    int
    listen(int socket, int backlog)
    {
        int rc;
        mock().actualCall("listen")
            .withParameter("socket", socket)
            .withOutputParameter("rc", &rc);
        return rc;
    }

    int
    pselect(int max_fd_count,
            fd_set *read_fd_set,
            fd_set *write_fd_set,
            fd_set *except_fd_set,
            const struct timespec *timeout,
	    const sigset_t * sigmask)
    {
        int num_ready_fds;
        int i;

        mock().actualCall("pselect")
            .withOutputParameter("num_ready_fds", &num_ready_fds);

        if (read_fd_set) {
            FD_ZERO(read_fd_set);
        }

        for (i = 0; i <= num_ready_fds; i++) {
            if (read_fd_set) {
                FD_SET(i, read_fd_set);
            }
        }

        return num_ready_fds;
    }

    int
    accept(int socket_fd,
           struct sockaddr *sockinfo,
           socklen_t *socklen)
    {
        int accepted_conn_fd;
        mock().actualCall("accept")
            .withOutputParameter("accepted_conn_fd", &accepted_conn_fd);
        return accepted_conn_fd;
    }

    ssize_t
    read(int fd_no,
         void *read_buffer,
         size_t buffer_len)
    {
        ssize_t num_bytes_read;
        mock().actualCall("read")
            .withOutputParameter("num_bytes_read", &num_bytes_read);
        return num_bytes_read;
    }
}
