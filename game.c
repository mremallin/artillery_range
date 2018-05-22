#include "game.h"
#include "text.h"
#include "main.h"
#include "object.h"
#include "line.h"
#include "colours.h"
#include "menu.h"
#include "game_ui.h"
#include "game_server.h"
#include "game_client.h"

#include <SDL.h>

static void
game_input_handler (SDL_Event *e)
{
    game_ui_handle_input(e);
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_RETURN) {
            SDL_Log("Back to main menu");
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
    struct sockaddr_in *server_bind_info;
    object_base_st *line;
    object_free_all_lists();
    game_install_api();

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

    game_server_start();
    server_bind_info = game_server_get_running_bind_info();
    while (server_bind_info->sin_port == 0) {}
    game_client_start(server_bind_info);
}

void
game_exit (void)
{
    game_client_finish();
    game_server_finish();
    object_free_all_lists();
    main_menu_start();
}
