#ifndef __SM_DEFS_H__
#define __SM_DEFS_H__

#include <stdint.h>

typedef uint8_t sm_event_t;
typedef uint8_t sm_state_t;
typedef void (*sm_cb_func)(void);

typedef struct sm_transition_st_ {
    sm_cb_func transition_cb;
    sm_state_t next_state;
} sm_transition_st;

typedef struct sm_state_base_st_ {
    sm_transition_st evt_array;
} sm_state_base_st;

typedef struct sm_base_st_ {
    sm_state_t current_state;
    sm_state_t num_states;
    sm_event_t num_events;
} sm_base_st;

typedef struct sm_generic_st_ {
    sm_base_st base;
    sm_state_base_st *sm_state_info;
} sm_generic_st;

#endif /* __SM_DEFS_H__ */
