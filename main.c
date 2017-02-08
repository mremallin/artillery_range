#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "display.h"
#include "charset.h"

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
}

#define FRAME_MS_60FPS (1000/60)
#define FRAME_MS_30FPS (1000/30)
#define MS_PER_SEC (1000)

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

static uint32_t scroll_x = 0;
#define TEXT_SCROLL_SPEED_PX_MS (60 * 1000)

static void
update_frame (uint32_t frame_delta_ms)
{
    scroll_x += (TEXT_SCROLL_SPEED_PX_MS / frame_delta_ms) / MS_PER_SEC;
    SDL_Log("Scroll x: %u\n", scroll_x);
    if (scroll_x == SCREEN_WIDTH) {
        scroll_x = 0;
    }
    display_start_frame();
    display_set_text_cursor(scroll_x, SCREEN_HEIGHT/4);
    display_printf("I scroll!");
    display_set_text_cursor(0, 0);
    display_printf("!\"#$%%&'()*+,-./0123456789:;<=>?@"
                   "ABCDEFGHI                   JKLMNOPQRSTUVWXYZ");
    display_set_text_cursor(0, SCREEN_HEIGHT/2);
    display_printf_centred_x("Hello User!");
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

    /* Start the game! */
    main_event_loop();

    /* Exit */
    display_shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
