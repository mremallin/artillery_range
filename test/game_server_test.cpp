#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "common_defs.h"

extern "C" {
#include "game_server.c"
}

TEST_GROUP(game_server_ext_api)
{
    void
    teardown (void)
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST_GROUP(game_server_listen_socket)
{
    void
    teardown (void)
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(game_server_ext_api, get_bind_info)
{
    CHECK_EQUAL(game_server_get_running_bind_info(),
                &s_server_sockaddr);
}

TEST(game_server_ext_api, start_success)
{
    int rc = 0;

    mock().expectOneCall("pthread_create")
        .withParameter("thread", &s_game_server_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));

    game_server_start();
    CHECK_EQUAL(s_game_server_running, true);
}

TEST(game_server_ext_api, start_pthread_create_failure)
{
    int rc = EAGAIN;

    mock().expectOneCall("pthread_create")
        .withParameter("thread", &s_game_server_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);

    CHECK_THROWS(std::exception, game_server_start());
    CHECK_EQUAL(s_game_server_running, false);
}

TEST(game_server_ext_api, finish_success)
{
    int rc = 0;

    mock().expectOneCall("pthread_join")
        .withParameter("thread", s_game_server_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));

    game_server_finish();
    CHECK_EQUAL(s_game_server_running, false);
}

TEST(game_server_ext_api, finish_pthread_join_failure)
{
    int rc = ENOENT;

    mock().expectOneCall("pthread_join")
        .withParameter("thread", s_game_server_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));

    game_server_finish();
    CHECK_EQUAL(s_game_server_running, false);
}

TEST(game_server_listen_socket, open_socket_success)
{
    int socket_no = TEST_FD_NO;
    int rc = 0;

    mock().expectOneCall("socket")
        .withParameter("domain", AF_INET)
        .withParameter("type", SOCK_STREAM)
        .withParameter("protocol", IPPROTO_TCP)
        .withOutputParameterReturning("rc", &socket_no, sizeof(socket_no));

    mock().expectOneCall("bind")
        .withParameter("socket", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("listen")
        .withParameter("socket", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    game_server_open_socket();
}

TEST(game_server_listen_socket, open_socket_socket_failure)
{
    int socket_no = -1;

    mock().expectOneCall("socket")
        .withParameter("domain", AF_INET)
        .withParameter("type", SOCK_STREAM)
        .withParameter("protocol", IPPROTO_TCP)
        .withOutputParameterReturning("rc", &socket_no, sizeof(socket_no));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);

    CHECK_THROWS(std::exception, game_server_open_socket());
    CHECK_EQUAL(-1, s_game_server_receive_socket_v4);
}

TEST(game_server_listen_socket, open_socket_bind_failure)
{
    int socket_no = TEST_FD_NO;
    int rc = -1;

    mock().expectOneCall("socket")
        .withParameter("domain", AF_INET)
        .withParameter("type", SOCK_STREAM)
        .withParameter("protocol", IPPROTO_TCP)
        .withOutputParameterReturning("rc", &socket_no, sizeof(socket_no));

    mock().expectOneCall("bind")
        .withParameter("socket", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("close")
        .withParameter("fd", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);
    CHECK_THROWS(std::exception, game_server_open_socket());
}

TEST(game_server_listen_socket, open_socket_listen_failure)
{
    int socket_no = TEST_FD_NO;
    int rc = 0;
    int listen_rc = -1;

    mock().expectOneCall("socket")
        .withParameter("domain", AF_INET)
        .withParameter("type", SOCK_STREAM)
        .withParameter("protocol", IPPROTO_TCP)
        .withOutputParameterReturning("rc", &socket_no, sizeof(socket_no));

    mock().expectOneCall("bind")
        .withParameter("socket", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("listen")
        .withParameter("socket", TEST_FD_NO)
        .withOutputParameterReturning("rc", &listen_rc, sizeof(listen_rc));
    mock().expectOneCall("close")
        .withParameter("fd", TEST_FD_NO)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);
    CHECK_THROWS(std::exception, game_server_open_socket());
}
