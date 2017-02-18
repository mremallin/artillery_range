#include "line.h"

#include <assert.h>
#include <stdbool.h>

#include "object.h"
#include "display.h"
#include "utils.h"
#include "main.h"

static void
line_update_position (object_base_st *base,
                      uint32_t frame_tick_ms)
{
}

static void
line_draw_vertical (line_obj_st *line)
{
    uint32_t *display_buffer = display_get_buffer();
    uint32_t starting_y = MIN(line->base.y, line->y2);
    uint32_t ending_y = MAX(line->base.y, line->y2);
    uint32_t draw_x = line->base.x;
    uint32_t i;

    for (i = starting_y; i < ending_y; i++) {
        display_buffer[draw_x +
                       (i * SCREEN_WIDTH)] = line->colour;
    }
}

static void
line_draw_horizontal (line_obj_st *line)
{
    uint32_t *display_buffer = display_get_buffer();
    uint32_t starting_x = MIN(line->base.x, line->x2);
    uint32_t ending_x = MAX(line->base.x, line->x2);
    uint32_t draw_y = line->base.y;
    uint32_t i;

    for (i = starting_x; i < ending_x; i++) {
        display_buffer[i + (draw_y * SCREEN_WIDTH)] = line->colour;
    }

}

static void
line_draw (object_base_st *base)
{
    line_obj_st *line = (line_obj_st *)base;
    assert(line);

    if (line->base.x == line->x2) {
        /* Easy case for vertical line. */
        line_draw_vertical(line);
    } else if (line->base.y == line->y2) {
        /* Easy case for horizontal line. */
        line_draw_horizontal(line);
    } else {
        /* TODO: Support diagonal lines... */
        assert(false);
    }
}

static void
line_free (object_base_st *base)
{
    free(base);
}

static obj_api_st s_line_api = {
    .obj_upd_pos = line_update_position,
    .obj_draw = line_draw,
    .obj_destroy = line_free,
};

void
line_create (uint32_t x1,
             uint32_t y1,
             uint32_t x2,
             uint32_t y2,
             uint32_t colour,
             uint32_t flags)
{
    line_obj_st *line_obj = calloc(1, sizeof(line_obj_st));

    assert(line_obj);
    line_obj->base.x = x1;
    line_obj->base.y = y1;
    line_obj->base.obj_type = OBJECT_TYPE_TEXT;
    line_obj->base.obj_api = &s_line_api;
    line_obj->base.flags = flags;

    line_obj->x2 = x2;
    line_obj->y2 = y2;
    line_obj->colour = colour;

    if (line_obj->base.flags & LINE_FLAG_BACKGROUND) {
        object_add_to_list(&line_obj->base,
                           OBJECT_LIST_BACKGROUND);
    } else if (line_obj->base.flags & LINE_FLAG_MIDGROUND) {
        object_add_to_list(&line_obj->base,
                           OBJECT_LIST_MIDGROUND);
    } else if (line_obj->base.flags & LINE_FLAG_FOREGROUND) {
        object_add_to_list(&line_obj->base,
                           OBJECT_LIST_FOREGROUND);
    } else {
        assert(false);
    }
}
