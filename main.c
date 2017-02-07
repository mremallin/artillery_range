#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "display.h"

void
update_screen_buffer()
{
    uint32_t *buffer = display_get_buffer();
    uint32_t i = 0;

    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        buffer[i] = (i % 0xff) << 16 |
                    (i % 0x0f) << 8 |
                    (i % 0xf0);
    }
}

void
main_event_loop (void)
{
    SDL_Event e;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                SDL_Log("Got a quit event. Exiting...");
            }

            update_screen_buffer();
            display_finish_frame();
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
