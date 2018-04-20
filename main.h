#ifndef __MAIN_H__
#define __MAIN_H__

#include <SDL.h>

#include "sm.h"

#define PROG_VERSION 1
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAME_MS_60FPS (1000/60)
#define FRAME_MS_30FPS (1000/30)
#define MS_PER_SEC (1000)

#define DEBUG_MODE 1

typedef enum user_sdl_event_code_t_ {
    USER_EVENT_CALLBACK_TYPE = 0,
} main_sdl_event_code_t;

typedef void (*gm_input_handler_func)(SDL_Event *);
typedef struct main_game_mode_api_st_ {
    gm_input_handler_func input_handler;
} main_game_mode_api_st;

void
main_sm_intro_timer_exit(void);

void
main_sm_intro_exit(void);

void
main_sm_quit(void);

void
main_sm_install_game_mode_api(main_game_mode_api_st *);

void
main_sm_game_start(void);

void
main_sm_game_exit(void);

#endif /* __MAIN_H__ */
