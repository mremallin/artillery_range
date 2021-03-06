#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "display.h"
#include "charset.h"
#include "colours.h"

static SDL_Texture *screen_texture;
static SDL_Renderer *renderer;
static uint32_t *screen_buffer;

/* Text is drawn with the cursor being the upper left corner of the character */
static uint32_t text_cursor_x = 0;
static uint32_t text_cursor_y = 0;
static uint32_t text_colour = COLOUR_WHITE;

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
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED |
                                  SDL_RENDERER_PRESENTVSYNC);
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

    screen_buffer = calloc(SCREEN_WIDTH * SCREEN_HEIGHT,
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
display_start_frame (void)
{
    memset(screen_buffer, 0,
           SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
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

void
display_set_text_cursor (uint32_t x, uint32_t y)
{
    text_cursor_x = x;
    text_cursor_y = y;
}

uint32_t
display_get_text_cursor_x (void)
{
    return text_cursor_x;
}

uint32_t
display_get_text_cursor_y (void)
{
    return text_cursor_y;
}

uint32_t
display_get_text_colour (void)
{
    return text_colour;
}

void
display_set_text_colour (uint32_t argb)
{
    text_colour = argb;
}

static void
display_putc (char c)
{
    uint32_t char_x = text_cursor_x;
    uint32_t char_y = text_cursor_y;
    uint32_t char_draw_x = 0;
    uint32_t char_draw_y = 0;
    /* TODO: Don't support lower case. Force to upper. */
    uint8_t *char_bitmap = charset_get_char_bitmap(toupper(c));

    /* Special case for newlines. */
    if (c == '\n') {
        /* 8 pixels character height + 2 pixels spacing. */
        text_cursor_y += CHAR_NEWLINE_PIXELS_Y;
    }

    /* Need to handle screen wrapping as well. */
    if ((char_x + CHAR_WIDTH) >= SCREEN_WIDTH) {
        char_x = 0;
        text_cursor_x = 0;

        char_y += CHAR_NEWLINE_PIXELS_Y;
        text_cursor_y += CHAR_NEWLINE_PIXELS_Y;
    }

    for (char_draw_y = 0; char_draw_y < sizeof(uint8_t) * 8;
         char_draw_y++) {
        for (char_draw_x = 0; char_draw_x < sizeof(uint8_t) *8;
             char_draw_x++) {
            /* Draw a line of the bitmap. Each bit corresponds
             * to a single pixel on screen.  */
            if (char_bitmap[char_draw_y] & (1 << (7 - char_draw_x))) {
                screen_buffer[(char_x + char_draw_x) +
                              ((char_y+char_draw_y) * SCREEN_WIDTH)] =
                    text_colour;
            }
        }
    }

    text_cursor_x += (sizeof(uint8_t) * 8) + CHAR_KERNING_PIXELS_X;
}

void
display_printf_centred_x (const char *fmt, ...)
{
    char buffer[100];
    uint32_t i = 0, chars_printed = 0;
    va_list args;

    va_start(args, fmt);
    chars_printed = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    display_set_text_cursor((SCREEN_WIDTH -
                             (chars_printed *
                             (CHAR_WIDTH + CHAR_KERNING_PIXELS_X))) / 2,
                            display_get_text_cursor_y());

    while (buffer[i] != 0) {
        display_putc(buffer[i]);
        i++;
    }
}

void
display_printf (const char *fmt, ...)
{
    char buffer[100];
    uint32_t i = 0;
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    while (buffer[i] != 0) {
        display_putc(buffer[i]);
        i++;
    }
}
