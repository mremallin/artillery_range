#include "object.h"

#include <assert.h>

#include "main.h"
#include "text.h"
#include "utils.h"

static llist_head_st s_object_lists[OBJECT_LIST_MAX];

void
object_system_init (void)
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZEOF(s_object_lists); i++) {
        llist_init(&s_object_lists[i]);
    }
}

static void
object_update_walk_cb (llist_elem_st *elem,
                       void *ctx)
{
    uint32_t frame_tick_ms = *(uint32_t *)ctx;

    object_base_st *obj = (object_base_st *)elem;
    obj->obj_api->obj_upd_pos(obj, frame_tick_ms);
}

void
object_system_update (uint32_t frame_tick_ms)
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZEOF(s_object_lists); i++) {
        llist_walk(&s_object_lists[i],
                   object_update_walk_cb,
                   &frame_tick_ms);
    }
}

static void
object_draw_walk_cb (llist_elem_st *elem,
                     void *ctx)
{
    object_base_st *obj = (object_base_st *)elem;
    obj->obj_api->obj_draw(obj);
}

void
object_system_draw ()
{
    int i = 0;

    for (i = 0; i < ARRAY_SIZEOF(s_object_lists); i++) {
        llist_walk(&s_object_lists[i],
                   object_draw_walk_cb,
                   NULL);
    }
}

void
object_add_to_list (object_base_st *obj,
                    object_list_et obj_list)
{
    assert(obj_list < OBJECT_LIST_MAX);

    llist_append(&s_object_lists[obj_list],
                 &obj->elem);
}

void
object_del_walk_cb (llist_elem_st *elem,
                    void *ctx)
{
    object_base_st *obj = (object_base_st *)elem;
    obj->obj_api->obj_destroy(obj);
}

void
object_clear_list (object_list_et obj_list)
{
    assert(obj_list < OBJECT_LIST_MAX);
    llist_walk_destroy(&s_object_lists[obj_list],
                       object_del_walk_cb,
                       NULL);
}
