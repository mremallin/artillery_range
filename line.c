#include "line.h"

#include <assert.h>
#include <stdbool.h>

#include "object.h"
#include "display.h"
#include "utils.h"
#include "main.h"
#include "colours.h"

struct line_obj_st_ {
    object_base_st base;
    uint32_t x2;
    uint32_t y2;
    argb_colour line_colour;
};

static void
line_update_position (object_base_st *base,
                      uint32_t frame_tick_ms)
{
}

static void
line_draw_bresenham (struct line_obj_st_ *line)
{
    uint32_t *display_buffer = display_get_buffer();
    uint32_t dx = line->base.width - line->base.x;
    uint32_t dy = line->base.height - line->base.y;
    uint32_t x, y;
    uint32_t eps = 0;

    if (line->base.width - line->base.x >= line->base.height - line->base.y) {
        y = line->base.y;
        for (x = line->base.x; x <= line->base.width; x++) {
            display_buffer[x + (y * SCREEN_WIDTH)] = line->line_colour;
            eps += dy;
            if ((eps << 1) >= dx) {
                y++;
                eps -= dx;
            }
        }
    } else {
        x = line->base.x;
        for (y = line->base.y; y <= line->base.height; y++) {
            display_buffer[x + (y * SCREEN_WIDTH)] = line->line_colour;
            eps += dx;
            if ((eps << 1) >= dy) {
                x++;
                eps -= dy;
            }
        }
    }
}

static void
line_draw_vertical (struct line_obj_st_ *line)
{
    uint32_t *display_buffer = display_get_buffer();
    uint32_t starting_y = MIN(line->base.y, line->base.height);
    uint32_t ending_y = MAX(line->base.y, line->base.height);
    uint32_t draw_x = line->base.x;
    uint32_t i;

    for (i = starting_y; i < ending_y; i++) {
        display_buffer[draw_x +
                       (i * SCREEN_WIDTH)] = line->line_colour;
    }
}

static void
line_draw_horizontal (struct line_obj_st_ *line)
{
    uint32_t *display_buffer = display_get_buffer();
    uint32_t starting_x = MIN(line->base.x, line->base.width);
    uint32_t ending_x = MAX(line->base.x, line->base.width);
    uint32_t draw_y = line->base.y;
    uint32_t i;

    for (i = starting_x; i < ending_x; i++) {
        display_buffer[i + (draw_y * SCREEN_WIDTH)] = line->line_colour;
    }
}

static void
line_draw (object_base_st *base)
{
    struct line_obj_st_ *line = (struct line_obj_st_ *)base;
    assert(line);

    if (line->base.x == line->base.width) {
        /* Easy case for vertical line. */
        line_draw_vertical(line);
    } else if (line->base.y == line->base.height) {
        /* Easy case for horizontal line. */
        line_draw_horizontal(line);
    } else {
        /* TODO: Support diagonal lines... */
        line_draw_bresenham(line);
    }
}

static void
line_free (object_base_st *base)
{
    assert(base->obj_type == OBJECT_TYPE_LINE);
    free(base);
}

static obj_api_st s_line_api = {
    .obj_upd_pos = line_update_position,
    .obj_draw = line_draw,
    .obj_destroy = line_free,
};

object_base_st *
line_create (uint32_t x1,
             uint32_t y1,
             uint32_t x2,
             uint32_t y2,
             uint32_t colour)
{
    struct line_obj_st_ *line_obj = calloc(1, sizeof(struct line_obj_st_));

    assert(line_obj);
    line_obj->base.x = x1;
    line_obj->base.y = y1;
    line_obj->base.width = x2;
    line_obj->base.height = y2;
    line_obj->base.obj_type = OBJECT_TYPE_LINE;
    line_obj->base.obj_api = &s_line_api;

    line_obj->line_colour = colour;

    return &line_obj->base;
}
