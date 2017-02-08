#include "object.h"

#include <assert.h>

#include "main.h"
#include "display.h"

static obj_api_st obj_api;
static object_base_st test_obj;

static uint32_t scroll_x = 0;
#define TEXT_SCROLL_SPEED_PX_MS (60 * 1000)

void
test_object_update_position (object_base_st *base,
                             uint32_t        frame_tick_ms)
{
    assert(base->obj_type == OBJECT_TYPE_TEXT);
    base->x += (TEXT_SCROLL_SPEED_PX_MS / frame_tick_ms) / MS_PER_SEC;
    if (base->x >= SCREEN_WIDTH) {
        base->x = 0;
    }
}

void
test_object_draw (object_base_st *base)
{
    display_set_text_cursor(base->x, base->y);
    display_printf("I scroll!");
}

void
object_system_init (void)
{
    memset(&test_obj, 0, sizeof(test_obj));
    test_obj.obj_type = OBJECT_TYPE_TEXT;
    test_obj.x = 0;
    test_obj.y = SCREEN_HEIGHT/4;
    test_obj.obj_api = &obj_api;

    obj_api.obj_upd_pos = &test_object_update_position;
    obj_api.obj_draw = &test_object_draw;
}

void
object_system_update (uint32_t frame_tick_ms)
{
    test_obj.obj_api->obj_upd_pos(&test_obj,
                               frame_tick_ms);
    test_obj.obj_api->obj_draw(&test_obj);
}
