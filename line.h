#ifndef __LINE_H__
#define __LINE_H__

#include "object.h"
#include "colours.h"

object_base_st *
line_create(uint32_t x1,
            uint32_t y1,
            uint32_t x2,
            uint32_t y2,
            argb_colour colour);

#endif /* __LINE_H__ */
