/* cursor.c - Functions to update cursor
 * vim:ts=4 noexpandtab
 */

#include "cursor.h"
#include "i8259.h"
#include "lib.h"

// code from https://wiki.osdev.org/Text_Mode_Cursor

/* enable_cursor
 *  Functionality: Enables the cursor on screen with
 *                 size depending on the start and end
 *                 scanlines (rows).
 *  Arguments: cursor_start: highest scanline for cursor
 *             cursor_end: lowest scan ine for cursor
 *  Return: none
 *  References: https://wiki.osdev.org/Text_Mode_Cursor
 */
void enable_cursor(int cursor_start, int cursor_end){

    /* highest scanline = 0, lowest scanline = 15 */

    outb(CURS_START_R, CRTC_REG_1);
	outb((inb(CRTC_REG_2) & C_MASK) | cursor_start, CRTC_REG_2);
 
	outb(CURS_END_R, CRTC_REG_1);
	outb((inb(CRTC_REG_2) & E_MASK) | cursor_end, CRTC_REG_2);
}

/* disable_cursor
 *  Functionality: Disables the cursor - not used right now
 *  Arguments: none
 *  Return: none
 *  References: https://wiki.osdev.org/Text_Mode_Cursor
 */
void disable_cursor(){
    outb(CURS_START_R, CRTC_REG_1);
	outb(DISABLE, CRTC_REG_2);
}

/* update_cursor
 *  Functionality: Updates the cursor on screen
 *                 based on the current x and y 
 *                 coordinate.
 *  Arguments: screen_x: current x position on screen
 *             screen_y: current y position on screen
 *  Return: none
 *  References: https://wiki.osdev.org/Text_Mode_Cursor
 */
void update_cursor(int screen_x, int screen_y){
    uint16_t pos = screen_y * WIDTH_SCREEN + screen_x;
 
	outb(CURS_LOC_LOW, CRTC_REG_1);
	outb((uint8_t) (pos & FF_MASK), CRTC_REG_2);
	outb(CURS_LOC_HIGH, CRTC_REG_1);
	outb((uint8_t) ((pos >> EIGHT) & FF_MASK), CRTC_REG_2);
}

/* get_cursor_position
 *  Functionality: Gets the position of the cursor.
 *  Arguments: none
 *  Return: The position of the cursor currently.
 *  References: https://wiki.osdev.org/Text_Mode_Cursor
 */
int get_cursor_position(){
    uint16_t pos = 0;
    outb(CURS_LOC_LOW, CRTC_REG_1);
    pos |= inb(CRTC_REG_2);
    outb(CURS_LOC_HIGH, CRTC_REG_1);
    pos |= ((uint16_t)inb(CRTC_REG_2)) << EIGHT;
    return pos;
}
