#ifndef __SM_DEFS_H__
#define __SM_DEFS_H__

#include <stdint.h>

typedef uint8_t sm_event_t;
typedef uint8_t sm_state_t;
typedef void (*sm_cb_func)(void);

typedef struct sm_transition_st_ {
    sm_event_t incoming_event;
    sm_cb_func transition_cb;
    sm_state_t next_state;
} sm_transition_st;

#endif /* __SM_DEFS_H__ */
