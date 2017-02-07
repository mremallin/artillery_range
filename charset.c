#include <stdint.h>

static uint8_t charset[128][8] =
{
    /* Everyting uuld be pre-initialized to 0's */
    /* ' ' is 0's too, i'm starting after it. */
    ['!'] = {0b00011000,
             0b00111100,
             0b00111100,
             0b00111100,
             0b00011000,
             0b00000000,
             0b00011000,
             0b00011000},
    ['"'] = {0b00000000,
             0b01100110,
             0b01100110,
             0b01100110,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
    ['#'] = {0b01100110,
             0b01100110,
             0b11100111,
             0b11100111,
             0b01100110,
             0b11100111,
             0b11100111,
             0b01100110},
    ['$'] = {0b00011000,
             0b00011000,
             0b00111110,
             0b01011100,
             0b00111100,
             0b00011010,
             0b01111100,
             0b00011000},
    ['%'] = {0b00000001,
             0b01100010,
             0b01100100,
             0b00001000,
             0b00010000,
             0b00100110,
             0b01000110,
             0b10000000},
    ['&'] = {0b00111110,
             0b01000100,
             0b00101000,
             0b00010001,
             0b00101010,
             0b01000100,
             0b01111010,
             0b00000001},
    ['\''] = {0b00011000,
              0b00011000,
              0b00011000,
              0b00000000,
              0b00000000,
              0b00000000,
              0b00000000,
              0b00000000},
    ['('] = {0b00000100,
             0b00001000,
             0b00010000,
             0b00100000,
             0b00100000,
             0b00010000,
             0b00001000,
             0b00000100},
    [')'] = {0b00100000,
             0b00010000,
             0b00001000,
             0b00000100,
             0b00000100,
             0b00001000,
             0b00010000,
             0b00100000},
    ['*'] = {0b10011001,
             0b01011010,
             0b00111100,
             0b11111111,
             0b11111111,
             0b00111100,
             0b01011010,
             0b10011001},
    ['+'] = {0b00011000,
             0b00011000,
             0b00011000,
             0b11111111,
             0b11111111,
             0b00011000,
             0b00011000,
             0b00011000},
    [','] = {0b00000000,
             0b00000000,
             0b00000000,
             0b01100000,
             0b01100000,
             0b00010000,
             0b00010000,
             0b00100000},
    ['-'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b11111111,
             0b11111111,
             0b00000000,
             0b00000000,
             0b00000000},
    ['.'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00011000,
             0b00011000},
    ['/'] = {0b00000001,
             0b00000010,
             0b00000100,
             0b00001000,
             0b00010000,
             0b00100000,
             0b01000000,
             0b10000000},
    ['0'] = {0b00011000,
             0b00100100,
             0b01000110,
             0b10001001,
             0b10010001,
             0b01100010,
             0b00100100,
             0b00011000},
    ['1'] = {0b00011000,
             0b00111000,
             0b01011000,
             0b10011000,
             0b00011000,
             0b00011000,
             0b00011000,
             0b11111111},
    ['2'] = {0b00111000,
             0b01100100,
             0b11000100,
             0b00001000,
             0b00010000,
             0b00100000,
             0b01000000,
             0b11111111},
    /* TODO: Finish the charset. */
    ['3'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
    ['4'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
    ['5'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
    ['6'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
    ['7'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
    ['8'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
};

uint8_t *
charset_get_char_bitmap (char c)
{
    return charset[c];
}
