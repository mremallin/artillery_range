#include "text.h"

#include <stdbool.h>
#include <assert.h>

#include "main.h"
#include "display.h"
#include "charset.h"
#include "colours.h"

static void
text_update_position (object_base_st    *base,
                      uint32_t           frame_tick_ms)
{
    assert(base->obj_type == OBJECT_TYPE_TEXT);

    if (!OBJECT_TEST_FLAG(base, TEXT_OPTION_SCROLLING)) {
        return;
    }

    base->x += (TEXT_SCROLL_SPEED_PX_MS * frame_tick_ms) / MS_PER_SEC;
    if (base->x >= SCREEN_WIDTH) {
        base->x = 0;
    }
}

static void
text_putc (object_base_st *base, char c)
{
    text_obj_st *text = (text_obj_st *)base;
    uint32_t char_x = text->cursor_x;
    uint32_t char_y = text->cursor_y;
    uint32_t char_draw_x = 0;
    uint32_t char_draw_y = 0;
    /* TODO: Don't support lower case. Force to upper. */
    uint8_t *char_bitmap = charset_get_char_bitmap(toupper(c));
    uint32_t *display_buffer = display_get_buffer();

    /* Text objects don't support newlines right now */
    assert(c != '\n');

    /* Need to handle screen wrapping as well. */
    if ((char_x + CHAR_WIDTH) >= SCREEN_WIDTH) {
        if (OBJECT_TEST_FLAG(base, TEXT_OPTION_SCROLLING)) {
            char_x = 0;
        } else {
            char_x = 0;
            char_y += CHAR_NEWLINE_PIXELS_Y;
        }
    }

    for (char_draw_y = 0; char_draw_y < sizeof(uint8_t) * 8;
         char_draw_y++) {
        for (char_draw_x = 0; char_draw_x < sizeof(uint8_t) * 8;
             char_draw_x++) {
            /* Draw a line of the bitmap. Each bit corresponds
             * to a single pixel on screen.  */
            if (char_bitmap[char_draw_y] & (1 << (7 - char_draw_x))) {
                /* Scrolling means we need to wrap along the same line. */
                if (OBJECT_TEST_FLAG(base, TEXT_OPTION_SCROLLING)) {
                    if (char_x + char_draw_x >= SCREEN_WIDTH) {
                        char_x = 0;
                        if (text->cursor_x >= SCREEN_WIDTH) {
                            text->cursor_x = 0;
                        }
                    }
                    /* No alpha blending with text. */
                    display_buffer[(char_x + char_draw_x) +
                        ((char_y + char_draw_y) * SCREEN_WIDTH)] =
                        text->colour;
                } else {
                    /* No alpha blending with text. */
                    display_buffer[(char_x + char_draw_x) +
                        ((char_y + char_draw_y) * SCREEN_WIDTH)] =
                        text->colour;
                }
            }
        }
    }

    if (OBJECT_TEST_FLAG(base, TEXT_OPTION_SCROLLING)) {
        text->cursor_x += CHAR_WIDTH + CHAR_KERNING_PIXELS_X;
        text->cursor_x = text->cursor_x % SCREEN_WIDTH;
    } else {
        text->cursor_x += CHAR_WIDTH + CHAR_KERNING_PIXELS_X;
    }
}

void
text_print (text_obj_st *text)
{
    uint32_t i = 0;
    text->cursor_x = (uint32_t)text->base.x;
    text->cursor_y = (uint32_t)text->base.y;

    while (text->text[i] != 0) {
        text_putc(&text->base, text->text[i]);
        i++;
    }
}

void
text_draw (object_base_st   *base)
{
    text_obj_st *text;
    assert(base->obj_type == OBJECT_TYPE_TEXT);

    text = (text_obj_st *)base;
    display_set_text_cursor((uint32_t)base->x, (uint32_t)base->y);
    if (!OBJECT_TEST_FLAG(base, TEXT_OPTION_CENTRED_X)) {
        text_print(text);
    } else {
        display_printf_centred_x(text->text);
    }
}

void
text_free (object_base_st *base)
{
    free(base);
}

static obj_api_st s_text_api = {
    .obj_upd_pos = text_update_position,
    .obj_draw = text_draw,
    .obj_destroy = text_free,
};

void
text_create (uint32_t x,
             uint32_t y,
             char *text,
             uint32_t flags)
{
    text_obj_st *text_obj = calloc(1, sizeof(text_obj_st));

    assert(text_obj);
    text_obj->base.x = x;
    text_obj->base.y = y;
    text_obj->base.obj_type = OBJECT_TYPE_TEXT;
    text_obj->base.obj_api = &s_text_api;
    text_obj->base.flags = flags;

    text_obj->text = text;
    text_obj->colour = COLOUR_WHITE;

    object_add_to_list(&text_obj->base,
                       OBJECT_LIST_TEXT);
}
