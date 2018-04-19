#ifndef __LINE_H__
#define __LINE_H__

#include <stdint.h>

#include "object.h"

typedef struct line_obj_st_ line_obj_st;

object_base_st *
line_create(uint32_t x1,
            uint32_t y1,
            uint32_t x2,
            uint32_t y2,
            uint32_t colour);

#endif /* __LINE_H__ */
