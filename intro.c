#include "intro.h"
#include "text.h"
#include "menu.h"
#include "main.h"

#include <SDL.h>

#define INTRO_TIMER_LENGTH_MS (20 * 1000)

static void
intro_timer_evt_cb (void *ctx)
{
    main_sm_intro_timer_expired();
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
    userevent.code = 0;
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
    text_create(0, 0, "Intro goes here",
                TEXT_OPTION_SCROLLING);
    (void)SDL_AddTimer(INTRO_TIMER_LENGTH_MS,
                       intro_timer_queue_expiry_cb,
                       NULL);
}

void
intro_exit (void)
{
    text_destroy_all();
    main_menu_start();
}
