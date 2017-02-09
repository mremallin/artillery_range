#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "display.h"
#include "charset.h"
#include "object.h"
#include "sm.h"

void
intro_exit (void)
{
    SDL_Log("Transitioned out of the intro");
}

#include "game.sm"

static bool quitting = false;

static void
handle_events (SDL_Event *e)
{
    if (e == NULL) {
        return;
    }

    if (e->type == SDL_QUIT) {
        quitting = true;
        SDL_Log("Got a quit event. Exiting...");
    }

    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_RETURN) {
            sm_step_state(&sm_game.base, sm_game_event_KEY_PRESSED);
        }
    }
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

    while (!quitting) {
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

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
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

    /* Start the game! */
    main_event_loop();

    /* Exit */
    display_shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
