#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

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
