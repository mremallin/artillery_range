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
}
