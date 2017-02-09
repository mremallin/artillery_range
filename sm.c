#include "sm.h"

#include <string.h>
#include <SDL.h>

void
sm_step_state (sm_base_st *sm_base, sm_event_t evt)
{
    sm_generic_st *sm = (sm_generic_st *)sm_base;
    sm_transition_st *next_state;
    sm_state_base_st *state_array_base = (sm_state_base_st *)sm->sm_state_info;

    /* The generic SM info is basically an array of length
     * base->num_states (type sm_base_st). Within each sm_base_st,
     * there's another array of type sm_transition_st of length
     * base->num_events.
     *
     * There's a lot of macro trickery to make it compile each SM to it's own
     * type, but we can generalize it here to have a simple API
     * to use the SM.
     */

    /* Hackish way to generalize the state data as it's stored as a
     * static sized array when defined by the macros. The size of each
     * state is dependent on the number of events. In order
     * to access the correct state entry, we need to skip the appropriate
     * number of events considering one is already accounted for
     * by the structure sm_state_base_st.
     */
    next_state = &(&state_array_base[sm->base.current_state *
                                    (sm->base.num_events)].evt_array)[evt];

    if (next_state->transition_cb) {
        next_state->transition_cb();
    }

    sm->base.current_state = next_state->next_state;
}
