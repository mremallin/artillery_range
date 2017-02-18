#include "menu.h"
#include "text.h"
#include "main.h"
#include "charset.h"
#include "utils.h"
#include "sprite.h"

#include <assert.h>

#define MENU_SELECTOR_X_POSN                                                \
    (((SCREEN_WIDTH - charset_get_string_pixel_width(                       \
                        s_main_menu_entries[s_current_selection].menu_text))\
        / 2) - CHAR_WIDTH - CHAR_KERNING_PIXELS_X)

typedef void (*menu_entry_func)(void);
typedef struct main_menu_entry_st_ {
    char *menu_text;
    menu_entry_func menu_cb;
} main_menu_entry_st;

static uint32_t s_current_selection = 0;

void
menu_start_game (void)
{
    main_sm_game_start();
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
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_DOWN) {
            s_current_selection += 1;
            if (s_current_selection > ARRAY_SIZEOF(s_main_menu_entries)) {
                s_current_selection = 0;
            }
        }

        if (e->key.keysym.sym == SDLK_UP) {
            if (s_current_selection == 0) {
                s_current_selection = ARRAY_SIZEOF(s_main_menu_entries) - 1;
            } else {
                s_current_selection -= 1;
            }
        }

        if (e->key.keysym.sym == SDLK_RETURN) {
            s_main_menu_entries[s_current_selection].menu_cb();
        }
    }
}

static main_game_mode_api_st s_main_menu_api = {
    .input_handler = main_menu_input_handler,
};

static void
main_menu_install_api (void)
{
    main_sm_install_game_mode_api(&s_main_menu_api);
}

static void
main_menu_update_selection_sprite (sprite_obj_st *sprite)
{
    assert(sprite);

    sprite->base.x = MENU_SELECTOR_X_POSN;
    sprite->base.y = s_current_selection * CHAR_NEWLINE_PIXELS_Y;
}

void
main_menu_start (void)
{
    int i = 0;
    uint32_t y_offset = 0;
    s_current_selection = 0;

    main_menu_install_api();

    for (i = 0;
         i < ARRAY_SIZEOF(s_main_menu_entries);
         i++) {
        text_create(0, y_offset, s_main_menu_entries[i].menu_text,
                    TEXT_OPTION_CENTRED_X);
        y_offset += CHAR_NEWLINE_PIXELS_Y;
    }

    sprite_create(MENU_SELECTOR_X_POSN,
                  s_current_selection * CHAR_NEWLINE_PIXELS_Y,
                  charset_get_char_bitmap('>'),
                  CHAR_WIDTH,
                  CHAR_HEIGHT,
                  0xffffffff,
                  main_menu_update_selection_sprite,
                  SPRITE_FLAG_FOREGROUND);
}
