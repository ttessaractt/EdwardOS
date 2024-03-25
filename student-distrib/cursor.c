/* cursor.c - Functions to update cursor
 * vim:ts=4 noexpandtab
 */

#include "cursor.h"
#include "i8259.h"
#include "lib.h"

// code from https://wiki.osdev.org/Text_Mode_Cursor

void enable_cursor(int cursor_start, int cursor_end){
    outb(0x0A, 0x3D4);
	outb((inb(0x3D5) & 0xC0) | cursor_start, 0x3D5);
 
	outb(0x0B, 0x3D4);
	outb((inb(0x3D5) & 0xE0) | cursor_end, 0x3D5);
}

void disable_cursor(){
    outb(0x0A, 0x3D4);
	outb(0x20, 0x3D5);
}

void update_cursor(int screen_x, int screen_y){
    uint16_t pos = screen_y * WIDTH_SCREEN + screen_x;
 
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}

int get_cursor_position(){
    uint16_t pos = 0;
    outb(0x0F, 0x3D4);
    pos |= inb(0x3D5);
    outb(0x0E, 0x3D4);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

