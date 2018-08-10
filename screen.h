#ifndef SCREEN_H
#define SCREEN_H
#include "common.h"
#define COLOR_CODE 0x0a // light green

void screen_clear ();
void screen_put_char(const char c);
void screen_print (const char* const str);
void screen_print_hex (uint32_t number);

#endif
