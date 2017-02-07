#ifndef __CHARSET_H__
#define __CHARSET_H__

#define CHAR_NEWLINE_PIXELS_Y 10
#define CHAR_KERNING_PIXELS_X 1
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

uint8_t *
charset_get_char_bitmap(char c);

#endif /* __CHARSET_H__ */
