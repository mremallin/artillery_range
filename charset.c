#include <stdint.h>
#include <string.h>

#include "charset.h"

static uint8_t charset[128][CHAR_HEIGHT] =
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
             0b11111111,
             0b11111111,
             0b01100110,
             0b11111111,
             0b11111111,
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
             0b01001010,
             0b01010010,
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
             0b01100110,
             0b11000110,
             0b00001100,
             0b00011000,
             0b00110000,
             0b11111111,
             0b11111111},
    ['3'] = {0b01110000,
             0b01111100,
             0b00000100,
             0b01111100,
             0b01111100,
             0b00000100,
             0b01111100,
             0b01110000},
    ['4'] = {0b00011100,
             0b00111100,
             0b01101100,
             0b11001100,
             0b11111111,
             0b11111111,
             0b00001100,
             0b00001100},
    ['5'] = {0b01111111,
             0b01111111,
             0b01100000,
             0b01111000,
             0b01111100,
             0b00000110,
             0b01111110,
             0b01111110},
    ['6'] = {0b00011110,
             0b00111110,
             0b01100000,
             0b01111000,
             0b01100110,
             0b01100110,
             0b01100110,
             0b00111100},
    ['7'] = {0b11111111,
             0b11111111,
             0b00000110,
             0b00001100,
             0b00011000,
             0b00110000,
             0b01100000,
             0b11000000},
    ['8'] = {0b00111100,
             0b01100110,
             0b01100110,
             0b00111100,
             0b01100110,
             0b01100110,
             0b01100110,
             0b00111100},
    ['9'] = {0b00111110,
             0b01100110,
             0b01100110,
             0b00111110,
             0b00000110,
             0b00000110,
             0b0000110,
             0b00111000},
    [':'] = {0b00000000,
             0b00000000,
             0b00011000,
             0b00011000,
             0b00000000,
             0b00011000,
             0b00011000,
             0b00000000},
    [';'] = {0b00000000,
             0b00000000,
             0b00011000,
             0b00011000,
             0b00000000,
             0b00011000,
             0b00011000,
             0b00110000},
    ['<'] = {0b00000011,
             0b0000110,
             0b00110000,
             0b11000000,
             0b11000000,
             0b00110000,
             0b00001100,
             0b00000011},
    ['='] = {0b00000000,
             0b01111110,
             0b01111110,
             0b00000000,
             0b00000000,
             0b01111110,
             0b01111110,
             0b00000000},
    ['>'] = {0b11000000,
             0b00110000,
             0b00001100,
             0b00000011,
             0b00000011,
             0b00001100,
             0b00110000,
             0b11000000},
    ['?'] = {0b00111100,
             0b01100110,
             0b01100110,
             0b00001100,
             0b00011000,
             0b00000000,
             0b00011000,
             0b00011000},
    ['@'] = {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
    ['A'] = {0b00111100,
             0b01100110,
             0b01100110,
             0b01100110,
             0b01111110,
             0b01100110,
             0b01100110,
             0b01100110},
    ['B'] = {0b01111100,
             0b01100110,
             0b01100110,
             0b01111100,
             0b01100110,
             0b01100110,
             0b01100110,
             0b01111100},
    ['C'] = {0b00011110,
             0b00110000,
             0b01100000,
             0b11000000,
             0b11000000,
             0b01100000,
             0b00110000,
             0b00011110},
    ['D'] = {0b11111100,
             0b11001110,
             0b11000110,
             0b11000011,
             0b11000011,
             0b11000110,
             0b11001110,
             0b11111100},
    ['E'] = {0b11111111,
             0b11111111,
             0b11000000,
             0b11111111,
             0b11111111,
             0b11000000,
             0b11111111,
             0b11111111},
    ['F'] = {0b11111111,
             0b11111111,
             0b11000000,
             0b11111111,
             0b11111111,
             0b11000000,
             0b11000000,
             0b11000000},
    ['G'] = {0b00111100,
             0b01100110,
             0b11000000,
             0b11000000,
             0b11011110,
             0b11000110,
             0b01100110,
             0b00111100},
    ['H'] = {0b11000011,
             0b11000011,
             0b11000011,
             0b11111111,
             0b11111111,
             0b11000011,
             0b11000011,
             0b11000011},
    ['I'] = {0b11111111,
             0b11111111,
             0b00011000,
             0b00011000,
             0b00011000,
             0b00011000,
             0b11111111,
             0b11111111},
    ['J'] = {0b11111111,
             0b11111111,
             0b00011000,
             0b00011000,
             0b00011000,
             0b11011000,
             0b11011000,
             0b01110000},
    ['K'] = {0b11000110,
             0b11001100,
             0b11111000,
             0b11100000,
             0b11110000,
             0b11011000,
             0b11001100,
             0b11000110},
    ['L'] = {0b11000000,
             0b11000000,
             0b11000000,
             0b11000000,
             0b11000000,
             0b11000000,
             0b11111111,
             0b11111111},
    ['M'] = {0b11000011,
             0b11100111,
             0b11111111,
             0b11011011,
             0b11000011,
             0b11000011,
             0b11000011,
             0b11000011},
    ['N'] = {0b11000011,
             0b11110011,
             0b11110011,
             0b11011011,
             0b11001111,
             0b11001111,
             0b11000111,
             0b11000011},
    ['O'] = {0b00011000,
             0b00111100,
             0b01100110,
             0b11000011,
             0b11000011,
             0b01100110,
             0b00111100,
             0b00011000},
    ['P'] = {0b11111110,
             0b11000110,
             0b11000110,
             0b11000110,
             0b11111110,
             0b11000000,
             0b11000000,
             0b11000000},
    ['Q'] = {0b00000000,
             0b00111100,
             0b01100110,
             0b11000011,
             0b11000011,
             0b01101110,
             0b00111110,
             0b00000011},
    ['R'] = {0b11111110,
             0b11000110,
             0b11000110,
             0b11111110,
             0b11110000,
             0b11011000,
             0b11001100,
             0b11000110},
    ['S'] = {0b11111111,
             0b11111111,
             0b11000000,
             0b11111111,
             0b11111111,
             0b00000011,
             0b11111111,
             0b11111111},
    ['T'] = {0b11111111,
             0b11111111,
             0b00011000,
             0b00011000,
             0b00011000,
             0b00011000,
             0b00011000,
             0b00011000},
    ['U'] = {0b11000011,
             0b11000011,
             0b11000011,
             0b11000011,
             0b11000011,
             0b11000011,
             0b01100110,
             0b00111100},
    ['V'] = {0b11000011,
             0b11000011,
             0b11000011,
             0b01100110,
             0b01100110,
             0b00100100,
             0b00100100,
             0b00111100},
    ['W'] = {0b11000011,
             0b11000011,
             0b11000011,
             0b11000011,
             0b11011011,
             0b11111111,
             0b11100111,
             0b11000011},
    ['X'] = {0b11000011,
             0b01100110,
             0b01100110,
             0b00111100,
             0b00111100,
             0b01100110,
             0b01100110,
             0b11000011},
    ['Y'] = {0b11000011,
             0b01100110,
             0b01100110,
             0b00111100,
             0b00011000,
             0b00011000,
             0b00011000,
             0b00011000},
    ['Z'] = {0b11111111,
             0b11111111,
             0b00000110,
             0b00001100,
             0b00011000,
             0b00110000,
             0b11111111,
             0b11111111},
    /* TODO: Are any more characters needed? */
    ['['] = {0b00000000,
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
    return charset[(unsigned char)c];
}

size_t
charset_get_string_pixel_width (char *str)
{
    size_t strl = strlen(str);

    return (strl * CHAR_WIDTH) + (strl * CHAR_KERNING_PIXELS_X);
}
