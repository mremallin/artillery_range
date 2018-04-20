#include "game.h"
#include "text.h"
#include "main.h"
#include "object.h"
#include "line.h"
#include "colours.h"
#include "menu.h"
#include "game_ui.h"

#include <SDL.h>

static void
game_input_handler (SDL_Event *e)
{
    game_ui_handle_input(e);
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_RETURN) {
            main_sm_game_exit();
        }
    }
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
    object_base_st *line;
    object_free_all_lists();
    game_install_api();
    text_create(0, 0, "Game goes here",
            TEXT_OPTION_CENTRED_X);

    game_ui_create();

    line = line_create(200, 200, 200, 400, COLOUR_WHITE);
    object_add_to_list(line, OBJECT_LIST_BACKGROUND);
    line = line_create(200, 200, 400, 200, COLOUR_WHITE);
    object_add_to_list(line, OBJECT_LIST_BACKGROUND);
    line = line_create(200, 200, 400, 400, COLOUR_GREEN);
    object_add_to_list(line, OBJECT_LIST_BACKGROUND);
    line = line_create(200, 200, 300, 400, COLOUR_RED);
    object_add_to_list(line, OBJECT_LIST_BACKGROUND);
    line = line_create(200, 200, 400, 300, COLOUR_YELLOW);
    object_add_to_list(line, OBJECT_LIST_BACKGROUND);
}

void
game_exit (void)
{
    object_clear_list(OBJECT_LIST_TEXT);
    object_clear_list(OBJECT_LIST_BACKGROUND);
    main_menu_start();
}
