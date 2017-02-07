#include <assert.h>
#include <errno.h>

#include "main.h"
#include "display.h"

static SDL_Texture *screen_texture;
static SDL_Renderer *renderer;
static uint32_t *screen_buffer;

uint32_t *
display_get_buffer(void)
{
    assert(screen_buffer);
    return screen_buffer;
}

int
display_init (SDL_Window *window)
{
    assert(window);
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                     "Failed to create SDL renderer: %s",
                     SDL_GetError());
        return -1;
    }

    screen_texture = SDL_CreateTexture(renderer,
                                       SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       SCREEN_WIDTH, SCREEN_HEIGHT);
    if (screen_texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                     "Failed to create screen texture: %s",
                     SDL_GetError());
        return -1;
    }

    screen_buffer = calloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4,
                           sizeof(uint32_t));
    if (screen_buffer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                     "Failed to create screen buffer: %x",
                     ENOMEM);
        return -1;
    }

    return 0;
}

void
display_shutdown (void)
{
    free(screen_buffer);
    SDL_DestroyTexture(screen_texture);
    SDL_DestroyRenderer(renderer);
}

void
display_finish_frame (void)
{
    if (SDL_UpdateTexture(screen_texture, NULL, screen_buffer,
                          SCREEN_WIDTH * sizeof(uint32_t)) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                     "Failed to update the screen texture: %s\n",
                     SDL_GetError());
    }

    if (SDL_RenderClear(renderer) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                     "Failed to clear the renderer: %s\n",
                     SDL_GetError());
    }

    if (SDL_RenderCopy(renderer, screen_texture, NULL, NULL) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO,
                     "Failed to copy the texture to the renderer: %s\n",
                     SDL_GetError());
    }

    SDL_RenderPresent(renderer);
}
