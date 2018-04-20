#include "box.h"
#include "main.h"
#include "object.h"

static object_base_st *s_selector_box = NULL;
static uint32_t s_ui_selection = 0;

void
game_ui_handle_input (SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_RIGHT) {
            s_ui_selection += 1;
            if (s_ui_selection >= 10) {
                s_ui_selection = 0;
            }
        }
        if (e->key.keysym.sym == SDLK_LEFT) {
            if (s_ui_selection == 0) {
                s_ui_selection = 10 - 1;
            } else {
                s_ui_selection -= 1;
            }
        }
        s_selector_box->x = s_ui_selection * (SCREEN_WIDTH / 10);
    }
}

void
game_ui_create (void)
{
    object_base_st *box;
    uint32_t i;

    /* Overall bounding box */
    box = box_create(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/10, COLOUR_WHITE);
    object_add_to_list(box, OBJECT_LIST_FOREGROUND);

    for (i = 0; i <= SCREEN_WIDTH; i+=SCREEN_WIDTH/10) {
        box = box_create(i, 0, i+SCREEN_WIDTH/10, SCREEN_HEIGHT/10,
                         COLOUR_GREEN);
        object_add_to_list(box, OBJECT_LIST_FOREGROUND);
    }

    box = box_create(0, 0, SCREEN_WIDTH/10, SCREEN_HEIGHT/10, COLOUR_RED);
    s_selector_box = box;
    object_add_to_list(box, OBJECT_LIST_UI);
}
