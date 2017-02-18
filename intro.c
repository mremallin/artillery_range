#include "intro.h"
#include "text.h"
#include "menu.h"
#include "main.h"

#include <SDL.h>

#define INTRO_TIMER_LENGTH_MS (20 * 1000)

static void
intro_input_handler (SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_RETURN) {
            main_sm_intro_exit();
        }
    }
}

static main_game_mode_api_st s_intro_api = {
    .input_handler = intro_input_handler,
};

static void
intro_install_api (void)
{
    main_sm_install_game_mode_api(&s_intro_api);
}

static void
intro_timer_evt_cb (void *ctx)
{
    main_sm_intro_exit();
}

static uint32_t
intro_timer_queue_expiry_cb (uint32_t interval, void *param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    /* This CB is run in a different thread, so in order to
     * avoid multithreading issues, we requeue the event in
     * the main event loop for processing in the main thread. */
    userevent.type = SDL_USEREVENT;
    userevent.code = USER_EVENT_CALLBACK_TYPE;
    userevent.data1 = &intro_timer_evt_cb;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return interval;
}

void
intro_start (void)
{
    intro_install_api();
    text_create(0, 0, "Intro goes here",
                TEXT_OPTION_SCROLLING);
    (void)SDL_AddTimer(INTRO_TIMER_LENGTH_MS,
                       intro_timer_queue_expiry_cb,
                       NULL);
}

void
intro_exit (void)
{
    object_clear_list(OBJECT_LIST_TEXT);
    main_menu_start();
}
