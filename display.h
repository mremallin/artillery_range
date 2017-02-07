#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <SDL.h>

int
display_init(SDL_Window *window);

void
display_start_frame(void);

void
display_finish_frame(void);

void
display_shutdown(void);

void
display_set_text_cursor(uint32_t x, uint32_t y);

uint32_t
display_get_text_cursor_x(void);

uint32_t
display_get_text_cursor_y(void);

void
display_printf(const char *fmt, ...);

void
display_printf_centred(const char *fmt, ...);

#endif /* __DISPLAY_H__ */
