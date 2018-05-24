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

TEST_GROUP(game_client_main_loop)
{
    void
    teardown (void)
    {
        s_welcome_sent = false;
        mock().checkExpectations();
        mock().clear();
    }
};

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
    int rc = EAGAIN;
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

TEST(game_client_main_loop, write_once)
{
    ssize_t rc = 10;
    mock().expectOneCall("write")
        .withParameter("fd", s_game_client_server_socket_v4)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    game_client_main_loop();
    game_client_main_loop();
    game_client_main_loop();
}
