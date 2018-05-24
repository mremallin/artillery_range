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

TEST(game_server_ext_api, start_success)
{
    int rc = 0;

    mock().expectOneCall("pthread_create")
        .withParameter("thread", &s_game_server_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));

    game_server_start();
    CHECK(s_game_server_running);
}
