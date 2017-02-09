#ifndef __SM_H__
#define __SM_H__

#include "sm_defs.h"

/*    -> Event   | evt 1 | evt 2 | evt 3 |
 * Current State |
 * =======================================================
 *  state 1      | state 2 | state 1 | state 2
 *  state 2      | state 2 | state 2 | state 3
 *  state 3 
 */

/* Define state */
/* define transitions within (event, cb, next state) */

void
sm_step_state (sm_base_st *, sm_event_t);

#endif /* __SM_H__ */
