#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "display.h"

static bool quitting = false;
bool once = true;

static void
handle_system_event (SDL_Event *e)
{
    if (e->type == SDL_QUIT) {
        quitting = true;
        SDL_Log("Got a quit event. Exiting...");
    }
}

static void
update_frame (SDL_Event *e,
              uint32_t frame_delta_ticks)
{
    handle_system_event(e);
    display_start_frame();
    display_set_text_cursor(0, 0);
    display_printf("!\"#$%%&'()*+,-./0123456789:;<=>?@"
                   "ABCDEFGHI                   JKLMNOPQRSTUVWXYZ");
    display_set_text_cursor(0, SCREEN_HEIGHT/2);
    display_printf_centred("Hello User!");
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
        while (SDL_PollEvent(&e) != 0) {
            /* Bump the frame delta if we're too fast. */
            if (frame_delta_ticks < 1) {
                frame_delta_ticks = 1;
            }

            frame_start_ticks = SDL_GetTicks();
            update_frame(&e, frame_delta_ticks);
            frame_end_ticks = SDL_GetTicks();
            frame_delta_ticks = frame_end_ticks - frame_start_ticks;
        }
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

    /* Start the game! */
    main_event_loop();

    /* Exit */
    display_shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
