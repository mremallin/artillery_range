#include "object.h"

#include <assert.h>

#include "main.h"
#include "text.h"

void
object_system_init (void)
{
    text_system_init();
}

void
object_system_update (uint32_t frame_tick_ms)
{
    text_update_frame(frame_tick_ms);
}

void
object_system_draw ()
{
    text_draw_frame();
}
