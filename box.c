#include "box.h"
#include "display.h"
#include "main.h"
#include <assert.h>

static void
box_update_position (object_base_st *base,
                     uint32_t frame_tick_ms)
{
}

static void
box_draw (object_base_st *base)
{
    uint32_t *display_buffer = display_get_buffer();
    uint32_t i;

    /* Draw the top and bottom */
    for (i = base->x; i < (base->x + base->width); i++) {
        display_buffer[i + (base->y * SCREEN_WIDTH)] = base->colour;
        display_buffer[i + (base->height * SCREEN_WIDTH)] = base->colour;
    }

    /* Draw the sides */
    for (i = base->y; i < (base->y + base->height); i++) {
        display_buffer[base->x + (i * SCREEN_WIDTH)] = base->colour;
        display_buffer[base->x + base->width + (i * SCREEN_WIDTH)] = base->colour;
    }
}

static void
box_free (object_base_st *base)
{
    assert(base->obj_type == OBJECT_TYPE_BOX);
    free(base);
}

static obj_api_st s_box_api = {
    .obj_upd_pos = box_update_position,
    .obj_draw = box_draw,
    .obj_destroy = box_free,
};

object_base_st *
box_create (uint32_t x, uint32_t y,
            uint32_t width, uint32_t height,
            argb_colour box_colour)
{
    object_base_st *box_obj = calloc(1, sizeof(object_base_st));

    assert(box_obj);
    box_obj->x = x;
    box_obj->width = width;
    box_obj->y = y;
    box_obj->height = height;
    box_obj->obj_type = OBJECT_TYPE_BOX;
    box_obj->obj_api = &s_box_api;

    box_obj->colour = box_colour;
    return box_obj;
}
