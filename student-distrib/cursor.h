

#include "types.h"

#define WIDTH_SCREEN  80

void enable_cursor(int cursor_start, int cursor_end);

void disable_cursor();

void update_cursor(int screen_x, int screen_y);

int get_cursor_position();

