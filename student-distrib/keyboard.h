/* keyboard.h - Functions to initialize keyboard to work with 8259 and print keys to screen
 * vim:ts=4 noexpandtab
 */


// scan codes for numbers 0 to 9 and letters a to z

#include "types.h"

#define ONE_INDEX       2 // 0x2
#define ZERO_INDEX      11 // 0xB
#define EQUALS_INDEX    13 // 0xD
#define BACKSP_INDEX    14 // 0xE


#define Q_INDEX         16 // 0x10
#define P_INDEX         25 // 0x19
#define BRACKET_INDEX   27 // 0x1B


#define A_INDEX         30 // 0x1E
#define L_INDEX         38 // 0x26
#define TILDE_INDEX     41 // 0x29


#define BSLASH_INDEX    43 // 0x2B
#define Z_INDEX         44 // 0x2C
#define M_INDEX         50 // 0x32
#define QUESTION_INDEX  53 // 0x32


#define CAPS_INDEX      58 // 0x3A


#define KEYBOARD_DATA       0x60 // port to get scan code from keyboard


int good_index; // within bounds of characters to type

int CAPS_CHECK;

int SHIFT_CHECK;

int CTRL_CHECK;

int BACKSP_CHECK;

int terminal_can_read;

char keyboard_buffer[128];


int buffer_postion;





/* handles printing key pressed to screen */
extern void keyboard_handler();


/* initialize keyboard by enabling interrupts IRQ1 on pic */
extern void keyboard_init();




// terminal_open


// terminal_read
int32_t terminal_key_read(int32_t fd, char* buf, int32_t nbytes);

// terminal_write
int32_t terminal_key_write(int32_t fd, char* buf, int32_t nbytes);

// terminal_close


