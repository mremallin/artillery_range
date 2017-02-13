#include "menu.h"
#include "text.h"
#include "main.h"

static void
main_menu_input_handler (SDL_Event *e)
{
}

static main_game_mode_api_st s_main_menu_api = {
    .input_handler = main_menu_input_handler,
};

static void
main_menu_install_api (void)
{
    main_sm_install_game_mode_api(&s_main_menu_api);
}

void
main_menu_start (void)
{
    main_menu_install_api();
    text_create(0, 0, "Main menu goes here",
                TEXT_OPTION_CENTRED_X);
}
