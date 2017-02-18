#include "game.h"
#include "text.h"
#include "main.h"
#include "object.h"

#include <SDL.h>

static void
game_input_handler (SDL_Event *e)
{
}

static main_game_mode_api_st s_game_api = {
    .input_handler = game_input_handler,
};

static void
game_install_api (void)
{
    main_sm_install_game_mode_api(&s_game_api);
}

void
game_start (void)
{
    object_free_all_lists();
    game_install_api();
    text_create(0, 0, "Game goes here",
            TEXT_OPTION_CENTRED_X);
}

void
game_exit (void)
{
    object_clear_list(OBJECT_LIST_TEXT);
}
