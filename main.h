#ifndef __MAIN_H__
#define __MAIN_H__

#include "sm.h"

#define PROG_VERSION 1
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAME_MS_60FPS (1000/60)
#define FRAME_MS_30FPS (1000/30)
#define MS_PER_SEC (1000)

#define DEBUG_MODE 1

void
main_sm_intro_timer_expired (void);

#endif /* __MAIN_H__ */
