#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <stdarg.h>

extern "C" {
#include "game_client.c"
}

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
        printf("\n");
        vprintf(fmt, args);
        printf("\n");
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
}

TEST(game_client_ext_api, setup_pthread_failure)
{
    int rc = 1;
    mock().expectOneCall("pthread_create")
        .withParameter("thread", &s_game_client_thread)
        .withOutputParameterReturning("rc", &rc, sizeof(rc));
    mock().expectOneCall("exit")
        .withParameter("rc", EXIT_FAILURE);
    game_client_start(&connect_info);
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
