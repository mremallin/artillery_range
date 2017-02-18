#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "object.h"
#include "stdlib.h"

struct sprite_obj_st_;

typedef void (*sprite_position_update_func)(struct sprite_obj_st_ *);

typedef struct sprite_obj_st_ {
    object_base_st  base;
    void            *sprite_data;
    size_t          sprite_width;
    size_t          sprite_height;
    uint32_t        colour;
    sprite_position_update_func custom_pos_upd;
} sprite_obj_st;

#define SPRITE_FLAG_BACKGROUND  (1 << 0)
#define SPRITE_FLAG_MIDGROUND   (1 << 1)
#define SPRITE_FLAG_FOREGROUND  (1 << 2)

void
sprite_create(uint32_t x,
              uint32_t y,
              void *sprite_data,
              size_t sprite_width,
              size_t sprite_height,
              uint32_t colour,
              sprite_position_update_func custom_pos_upd,
              uint32_t flags);

#endif /* __SPRITE_H__ */
