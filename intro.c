#include "intro.h"
#include "text.h"

void
intro_start (void)
{
    text_create(0, 0, "Intro goes here",
                TEXT_OPTION_SCROLLING);
}

void
intro_exit (void)
{
    text_destroy_all();
    text_create(0, 0, "Main menu goes here",
                TEXT_OPTION_CENTRED_X);
}
