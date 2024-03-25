

#include "types.h"

#define WIDTH_SCREEN    80

#define CRTC_REG_1      0x3D4
#define CRTC_REG_2      0x3D5

/* CRTC registers */
#define CURS_START_R    0x0A
#define CURS_END_R      0x0B
#define CURS_LOC_HIGH   0x0E
#define CURS_LOC_LOW    0x0F

#define DISABLE         0x20

#define C_MASK          0xC0
#define E_MASK          0xE0
#define FF_MASK         0xFF

#define EIGHT           8

/* enables cursor */
void enable_cursor(int cursor_start, int cursor_end);

/* disables cursor */
void disable_cursor();

/* updates cursor location */
void update_cursor(int screen_x, int screen_y);

/* returns current cursor location */
int get_cursor_position();
