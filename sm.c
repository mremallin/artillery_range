#include "sm.h"

#include <string.h>

void
sm_init (sm_st *sm)
{
    memset(sm, 0, sizeof(*sm));
}

void
sm_step_state (sm_st *sm, sm_event_t evt)
{
}
