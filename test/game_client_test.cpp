#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <stdarg.h>
#include <exception>

extern "C" {
#include "game_client.c"
}

#define TEST_FD_NO 10

TEST_GROUP(game_client_ext_api)
{
    struct sockaddr_in connect_info = {
        .sin_len = 0x10,
        .sin_family = 0x02,
        .sin_port = 0x8fd1,
        .sin_addr = {
            .s_addr = 0x01020304,
        },
    };

    void
    teardown (void)
    {
        s_game_client_running = false;
        mock().checkExpectations();
        mock().clear();
    }
};

TEST_GROUP(game_client_socket)
{
    void
    setup (void)
    {
        s_client_sockaddr = {
            .sin_len = 0x10,
            .sin_family = 0x02,
            .sin_port = 0x8fd1,
            .sin_addr = {
                .s_addr = 0x01020304,
            },
        };

        s_game_client_server_socket_v4 = 0;
    }

    void
    teardown (void)
    {
        mock().checkExpectations();
        mock().clear();
    }
};

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
}

TEST(game_client_ext_api, setup_success)
{
    int rc = 0;

    mock().expectOneCall("pthread_create")
        .withParameter("thread", &s_game_client_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    game_client_start(&connect_info);

    CHECK_EQUAL(s_client_sockaddr.sin_family, connect_info.sin_family);
    CHECK_EQUAL(s_client_sockaddr.sin_port, connect_info.sin_port);
    CHECK_EQUAL(s_client_sockaddr.sin_addr.s_addr, connect_info.sin_addr.s_addr);
    CHECK(s_game_client_running);
}

TEST(game_client_ext_api, setup_pthread_failure)
{
    int rc = 1;
    mock().expectOneCall("pthread_create")
        .withParameter("thread", &s_game_client_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);

    CHECK_THROWS(std::exception, game_client_start(&connect_info));

    CHECK(s_game_client_running == false);
}

TEST(game_client_ext_api, finish_success)
{
    int rc = 0;

    s_game_client_thread = (pthread_t)0xdeadbeef;
    mock().expectOneCall("pthread_join")
        .withParameter("thread", s_game_client_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    game_client_finish();

    CHECK_EQUAL(s_client_sockaddr.sin_family, 0);
    CHECK_EQUAL(s_client_sockaddr.sin_port, 0);
    CHECK_EQUAL(s_client_sockaddr.sin_addr.s_addr, 0);
}

TEST(game_client_ext_api, finish_pthread_join_failure)
{
    int rc = 1;

    s_game_client_thread = (pthread_t)0xdeadbeef;
    mock().expectOneCall("pthread_join")
        .withParameter("thread", s_game_client_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    game_client_finish();

    CHECK_EQUAL(s_client_sockaddr.sin_family, 0);
    CHECK_EQUAL(s_client_sockaddr.sin_port, 0);
    CHECK_EQUAL(s_client_sockaddr.sin_addr.s_addr, 0);
}

TEST(game_client_socket, open_socket_success)
{
    int rc = TEST_FD_NO;

    mock().expectOneCall("socket")
        .withParameter("domain", AF_INET)
        .withParameter("type", SOCK_STREAM)
        .withParameter("protocol", IPPROTO_TCP)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));

    rc = 0;
    mock().expectOneCall("connect")
        .withParameter("socket", rc)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    game_client_open_socket();

    CHECK_EQUAL(s_game_client_server_socket_v4, rc);
}

TEST(game_client_socket, open_socket_socket_failure)
{
    int rc = -1;

    mock().expectOneCall("socket")
        .withParameter("domain", AF_INET)
        .withParameter("type", SOCK_STREAM)
        .withParameter("protocol", IPPROTO_TCP)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);

    CHECK_THROWS(std::exception, game_client_open_socket());
}

TEST(game_client_socket, open_socket_connect_failure)
{
    int rc = TEST_FD_NO;

    mock().expectOneCall("socket")
        .withParameter("domain", AF_INET)
        .withParameter("type", SOCK_STREAM)
        .withParameter("protocol", IPPROTO_TCP)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("connect")
        .withParameter("socket", rc)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("close")
        .withParameter("fd", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);

    CHECK_THROWS(std::exception, game_client_open_socket());
    CHECK_EQUAL(s_game_client_server_socket_v4, 0);
}

TEST(game_client_socket, close_socket_success)
{
    int rc = 0;
    s_game_client_server_socket_v4 = TEST_FD_NO;

    mock().expectOneCall("close")
        .withParameter("fd", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));

    game_client_close_socket();

    CHECK_EQUAL(s_game_client_server_socket_v4, 0);
}

TEST(game_client_socket, close_socket_failure)
{
    int rc = ENOENT;
    s_game_client_server_socket_v4 = TEST_FD_NO;

    mock().expectOneCall("close")
        .withParameter("fd", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));

    game_client_close_socket();

    CHECK_EQUAL(s_game_client_server_socket_v4, 0);
}
