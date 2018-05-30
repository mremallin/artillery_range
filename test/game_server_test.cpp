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

TEST_GROUP(game_server_network)
{
    void
    teardown (void)
    {
        size_t i;
        int rc = 0;

        for (i = 0; i < s_game_server_num_remote_conns; i++) {
            mock().expectOneCall("close")
                .withParameter("fd", s_game_server_remote_conns[i].gsrc_remote_fd)
                .withOutputParameterReturning("rc", &rc, sizeof(rc));
        }

        mock().expectOneCall("close")
            .withParameter("fd", s_game_server_receive_socket_v4)
            .withOutputParameterReturning("rc", &rc, sizeof(rc));
        game_server_close_socket();

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

TEST(game_server_network, handle_no_pending_connections)
{
    int num_ready_fds = 0;

    mock().expectOneCall("pselect")
        .withOutputParameterReturning("num_ready_fds", &num_ready_fds,
                                      sizeof(num_ready_fds));
    mock().expectOneCall("pselect")
        .withOutputParameterReturning("num_ready_fds", &num_ready_fds,
                                      sizeof(num_ready_fds));

    game_server_handle_network();
}

TEST(game_server_network, handle_one_pending_connection)
{
    int is_pending_conn = 1;
    int accepted_conn_fd = TEST_FD_NO;
    int num_ready_fds = 0;

    mock().expectOneCall("pselect")
        .withOutputParameterReturning("num_ready_fds", &is_pending_conn,
                                      sizeof(is_pending_conn));
    mock().expectOneCall("accept")
        .withOutputParameterReturning("accepted_conn_fd", &accepted_conn_fd,
                                      sizeof(accepted_conn_fd));

    mock().expectOneCall("pselect")
        .withOutputParameterReturning("num_ready_fds", &num_ready_fds,
                                      sizeof(num_ready_fds));

    game_server_handle_network();
    CHECK_EQUAL(s_game_server_num_remote_conns, 1);
    CHECK_EQUAL(s_game_server_remote_conns[0].gsrc_remote_fd, TEST_FD_NO);
}

TEST(game_server_network, handle_10_pending_connections)
{
    int is_pending_conn = 1;
    int accepted_conn_fd = TEST_FD_NO;
    int num_ready_fds = 0;
    int i;

    for (i = 0; i < 10; i++) {
        mock().expectOneCall("pselect")
            .withOutputParameterReturning("num_ready_fds", &is_pending_conn,
                                          sizeof(is_pending_conn));
        mock().expectOneCall("accept")
            .withOutputParameterReturning("accepted_conn_fd", &accepted_conn_fd,
                                          sizeof(accepted_conn_fd));
        mock().expectOneCall("pselect")
            .withOutputParameterReturning("num_ready_fds", &num_ready_fds,
                                          sizeof(num_ready_fds));
        game_server_handle_network();

        accepted_conn_fd++;
    }

    CHECK_EQUAL(s_game_server_num_remote_conns, 10);

    for(i = 0; i < 10; i++) {
        CHECK_EQUAL(s_game_server_remote_conns[i].gsrc_remote_fd,
                    TEST_FD_NO+i);
    }
}
