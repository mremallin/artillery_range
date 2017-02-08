#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdint.h>

#include "llist.h"

typedef enum object_type_et_ {
    OBJECT_TYPE_NONE = 0,
    OBJECT_TYPE_TEXT,
} object_type_et;

struct object_base_st_;

/* Takes an object, and frame tick delta. */
typedef void (*obj_upd_position_func)(struct object_base_st_ *, uint32_t);
/* Draws an object. Takes the base. */
typedef void (*obj_draw_func)(struct object_base_st_ *);

typedef struct obj_api_st_ {
    obj_upd_position_func   obj_upd_pos;
    obj_draw_func           obj_draw;
} obj_api_st;

#define OBJECT_TEST_FLAG(_base_p, _flag)                                    \
    (_base_p->flags & _flag)

typedef struct object_base_st_ {
    llist_elem_st elem;
    object_type_et obj_type;
    obj_api_st *obj_api;
    uint32_t x;
    uint32_t y;
    uint32_t flags;
} object_base_st;

void
object_system_init(void);

void
object_system_update(uint32_t frame_tick_ms);

void
object_system_draw();

#endif /* __OBJECT_H__ */
