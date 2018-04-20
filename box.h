#ifndef __BOX_H__
#define __BOX_H__

#include "object.h"
#include "colours.h"

object_base_st *
box_create (uint32_t x, uint32_t y,
            uint32_t width, uint32_t height,
            argb_colour box_colour);

#endif /* __BOX_H__ */
