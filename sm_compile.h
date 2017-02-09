#ifndef __SM_COMPILE_H__
#define __SM_COMPILE_H__

#include "sm_defs.h"

#define SM_EVENT_LIST_START(_name)                                          \
    typedef enum sm_event_list_##_name {                                    \
        sm_##_name##_event_min = 0,

#define SM_EVENT_LIST_ENTRY(_name, _evt_name)                               \
        sm_##_name##_event_##_evt_name,

#define SM_EVENT_LIST_END(_name)                                            \
        sm_##_name##_event_max,                                             \
    } sm_event_list_##_name##_t;

#define SM_STATE_LIST_START(_name)                                          \
    typedef enum sm_state_list_##_name {                                    \
        sm_##_name##_state_min = 0,

#define SM_STATE_LIST_ENTRY(_name, _st_name)                                \
        sm_##_name##_state_##_st_name,

#define SM_STATE_LIST_END(_sm_name)                                         \
        sm_##_sm_name##_state_max,                                          \
    } sm_state_list_##_sm_name##_t;                                         \
    typedef struct sm_state_##name_st_ {                                    \
        sm_transition_st evt_array[sm_##_sm_name##_event_max];              \
    } sm_state_##_sm_name##_st;

#define SM_START(_sm_name)                                                  \
    sm_state_##_sm_name##_st sm_transition_table_##_sm_name                 \
        [sm_##_sm_name##_state_max] = {                                     \
    [sm_##_sm_name##_state_min] = {                                         \
    },

#define SM_STATE_START(_sm_name, _sm_state)                                 \
    [sm_##_sm_name##_state_##_sm_state] = {                                 \
        .evt_array = {

#define SM_STATE_EVENT(_sm_name, _sm_evt, _sm_cb, _sm_next_state)           \
            [sm_##_sm_name##_event_##_sm_evt] = {                           \
                .incoming_event = sm_##_sm_name##_event_##_sm_evt,          \
                .transition_cb = _sm_cb,                                  \
                .next_state = sm_##_sm_name##_state_##_sm_next_state,       \
            },

#define SM_STATE_END()                                                      \
        }, },

#define SM_END(_sm_name)                                                    \
        };

#endif /* __SM_COMPILE_H__ */
