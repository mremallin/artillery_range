#include "sprite.h"

#include <assert.h>
#include <stdbool.h>

#include "display.h"
#include "main.h"

static void
sprite_update_position (object_base_st *base,
                        uint32_t frame_tick_ms)
{
    sprite_obj_st *sprite;
    assert(base->obj_type == OBJECT_TYPE_SPRITE);

    sprite = (sprite_obj_st *)base;

    if (sprite->custom_pos_upd) {
        sprite->custom_pos_upd(sprite);
    }
}

static void
sprite_draw (object_base_st *base)
{
    sprite_obj_st *sprite;
    uint32_t sprite_draw_x;
    uint32_t sprite_draw_y;
    uint32_t sprite_x;
    uint32_t sprite_y;
    uint8_t *sprite_data;
    uint32_t *display_buffer = display_get_buffer();

    assert(base->obj_type == OBJECT_TYPE_SPRITE);

    sprite = (sprite_obj_st *)base;
    /* Only 8px wide sprite for now. */
    assert(sprite->sprite_width == 8);
    sprite_data = sprite->sprite_data;
    sprite_x = sprite->base.x;
    sprite_y = sprite->base.y;

    /* TODO: Alpha blending? */
    for (sprite_draw_y = 0; sprite_draw_y < sprite->sprite_height;
         sprite_draw_y++) {
        for (sprite_draw_x = 0; sprite_draw_x < sprite->sprite_width;
             sprite_draw_x++) {
            if (sprite_data[sprite_draw_y] & (1 << (7 - sprite_draw_x))) {
                display_buffer[(sprite_x + sprite_draw_x) +
                               ((sprite_y + sprite_draw_y) * SCREEN_WIDTH)] =
                               sprite->colour;
            }
        }
    }
}

static void
sprite_free (object_base_st *base)
{
    free(base);
}

static obj_api_st s_sprite_api = {
    .obj_upd_pos = sprite_update_position,
    .obj_draw = sprite_draw,
    .obj_destroy = sprite_free,
};

void
sprite_create (uint32_t x,
               uint32_t y,
               void *sprite_data,
               size_t sprite_width,
               size_t sprite_height,
               uint32_t colour,
               sprite_position_update_func custom_pos_upd,
               uint32_t flags)
{
    sprite_obj_st *sprite_obj = calloc(1, sizeof(sprite_obj_st));

    assert(sprite_obj);
    sprite_obj->base.x = x;
    sprite_obj->base.y = y;
    sprite_obj->base.obj_type = OBJECT_TYPE_SPRITE;
    sprite_obj->base.obj_api = &s_sprite_api;
    sprite_obj->base.flags = flags;

    sprite_obj->sprite_data = sprite_data;
    sprite_obj->sprite_width = sprite_width;
    sprite_obj->sprite_height = sprite_height;
    sprite_obj->colour = colour;
    sprite_obj->custom_pos_upd = custom_pos_upd;

    if (sprite_obj->base.flags & SPRITE_FLAG_BACKGROUND) {
        object_add_to_list(&sprite_obj->base,
                           OBJECT_LIST_BACKGROUND);
    } else if (sprite_obj->base.flags & SPRITE_FLAG_MIDGROUND) {
        object_add_to_list(&sprite_obj->base,
                           OBJECT_LIST_MIDGROUND);
    } else if (sprite_obj->base.flags & SPRITE_FLAG_FOREGROUND) {
        object_add_to_list(&sprite_obj->base,
                           OBJECT_LIST_FOREGROUND);
    } else {
        assert(false);
    }
}
