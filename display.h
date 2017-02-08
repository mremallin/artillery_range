#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <SDL.h>

#define COLOUR_GREEN    0xff00ff00
#define COLOUR_YELLOW   0xffffff00
#define COLOUR_RED      0xffff0000

int
display_init(SDL_Window *window);

void
display_start_frame(void);

void
display_finish_frame(void);

void
display_shutdown(void);

uint32_t *
display_get_buffer();

void
display_set_text_cursor(uint32_t x, uint32_t y);

uint32_t
display_get_text_cursor_x(void);

uint32_t
display_get_text_cursor_y(void);

uint32_t
display_get_text_colour();

void
display_set_text_colour(uint32_t argb);

void
display_printf(const char *fmt, ...);

void
display_printf_centred_x(const char *fmt, ...);

#endif /* __DISPLAY_H__ */
