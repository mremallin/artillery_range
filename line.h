#ifndef __LINE_H__
#define __LINE_H__

#include <stdint.h>

#include "object.h"

#define LINE_FLAG_BACKGROUND    (1 << 0)
#define LINE_FLAG_MIDGROUND     (1 << 1)
#define LINE_FLAG_FOREGROUND    (1 << 2)

typedef struct line_obj_st_ {
    object_base_st base;
    float x2;
    float y2;
    uint32_t colour;
} line_obj_st;

void
line_create(uint32_t x1,
            uint32_t y1,
            uint32_t x2,
            uint32_t y2,
            uint32_t colour,
            uint32_t flags);

#endif /* __LINE_H__ */
