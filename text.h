#ifndef __TEXT_H__
#define __TEXT_H__

#include <stdint.h>
#include <stdbool.h>

#include "object.h"

#define TEXT_SCROLL_SPEED_PX_MS (60.0f)

#define TEXT_OPTION_NONE        (0)
#define TEXT_OPTION_SCROLLING   (1 << 0)
#define TEXT_OPTION_CENTRED_X   (1 << 1)

typedef struct text_obj_st_ {
    object_base_st base;
    char *text;
    uint32_t flags;
    uint32_t colour;
    uint32_t cursor_x;
    uint32_t cursor_y;
} text_obj_st;

void
text_create(uint32_t x,
            uint32_t y,
            char *text,
            uint32_t flags);

#endif /* __TEXT_H__ */
