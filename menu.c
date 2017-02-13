#include "menu.h"
#include "text.h"
#include "main.h"
#include "charset.h"

typedef void (*menu_entry_func)(void);
typedef struct main_menu_entry_st_ {
    char *menu_text;
    menu_entry_func menu_cb;
} main_menu_entry_st;

void
menu_start_game (void)
{
}

void
menu_quit_game (void)
{
    main_sm_quit();
}

static main_menu_entry_st s_main_menu_entries[] = {
    {.menu_text = "Start Game", .menu_cb = menu_start_game},
    {.menu_text = "Quit", .menu_cb = menu_quit_game},
};

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
    int i = 0;
    uint32_t y_offset = 0;

    main_menu_install_api();

    for (i = 0;
         i < sizeof(s_main_menu_entries) / sizeof(s_main_menu_entries[0]);
         i++) {
        text_create(0, y_offset, s_main_menu_entries[i].menu_text,
                    TEXT_OPTION_CENTRED_X);
        y_offset += CHAR_NEWLINE_PIXELS_Y;
    }
}
