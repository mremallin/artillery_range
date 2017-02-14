#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "main.h"
#include "display.h"
#include "charset.h"
#include "object.h"
#include "sm.h"
#include "text.h"
#include "intro.h"

#include "game.sm"

static bool s_quitting = false;
static main_game_mode_api_st *s_current_mode_api = NULL;

void
main_sm_quit (void)
{
    s_quitting = true;
    SDL_Log("Got a quit event. Exiting...");
}

static void
handle_events (SDL_Event *e)
{
    if (e == NULL) {
        return;
    }

    if (e->type == SDL_QUIT) {
        main_sm_quit();
    }

    if (e->type == SDL_KEYDOWN) {
        if (s_current_mode_api && s_current_mode_api->input_handler) {
            s_current_mode_api->input_handler(e);
        }

        if (e->key.keysym.sym == SDLK_RETURN) {
            sm_step_state(&sm_game.base, sm_game_event_KEY_PRESSED);
        }
    }

    if (e->type == SDL_USEREVENT) {
        switch (e->user.code) {
            case USER_EVENT_CALLBACK_TYPE: {
                void (*p)(void *) = e->user.data1;
                p(e->user.data2);
                break;
            }
            default:
                /* Catch any unhandled events. */
                assert(false);
        }
    }
}

void
main_sm_intro_timer_expired (void)
{
    sm_step_state(&sm_game.base, sm_game_event_INTRO_TIMER_EXPIRY);
}

void
main_sm_intro_key_pressed (void)
{
    sm_step_state(&sm_game.base, sm_game_event_KEY_PRESSED);
}

void
main_sm_install_game_mode_api (main_game_mode_api_st *api)
{
    s_current_mode_api = api;
}

static void
display_framerate (uint32_t frame_ticks_ms)
{
    uint32_t old_text_colour = display_get_text_colour();

    display_set_text_cursor(0, SCREEN_HEIGHT - CHAR_HEIGHT);

    if (frame_ticks_ms <= FRAME_MS_60FPS) {
        display_set_text_colour(COLOUR_GREEN);
    } else if (frame_ticks_ms > FRAME_MS_60FPS &&
               frame_ticks_ms < FRAME_MS_30FPS) {
        display_set_text_colour(COLOUR_YELLOW);
    } else {
        display_set_text_colour(COLOUR_RED);
    }

    display_printf("Frame ticks: %u", frame_ticks_ms);
    display_set_text_colour(old_text_colour);
}

static void
update_frame (uint32_t frame_delta_ms)
{
    display_start_frame();
    object_system_update(frame_delta_ms);
    object_system_draw();
#ifdef DEBUG_MODE
    display_framerate(frame_delta_ms);
#endif
    display_finish_frame();
}

static void
main_event_loop (void)
{
    SDL_Event e;
    uint32_t frame_start_ticks = 0;
    uint32_t frame_end_ticks = 0;
    uint32_t frame_delta_ticks = 0;

    while (!s_quitting) {
        /* Bump the frame delta if we're too fast. */
        if (frame_delta_ticks < 1) {
            frame_delta_ticks = 1;
        }

        frame_start_ticks = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0) {
            handle_events(&e);
        }

        update_frame(frame_delta_ticks);
        frame_end_ticks = SDL_GetTicks();
        frame_delta_ticks = frame_end_ticks - frame_start_ticks;
    }
}

int
main (int argc, char *argv[])
{
    SDL_Window *window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL could not initialize! SDL_Error: %s",
                     SDL_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Artillery Range", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Window could not be created! SDL_Error: %s",
                     SDL_GetError());
        return -1;
    }

    if (display_init(window)) {
        /* Error logging handled within display_init */
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                     "Failed to initialize the display. Exiting...");
        return -1;
    }

    object_system_init();
    sm_step_state(&sm_game.base, sm_game_event_STARTUP_COMPLETE);

    /* Start the game! */
    main_event_loop();

    /* Exit */
    display_shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
