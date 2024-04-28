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
#define CTRL_INDEX      29 // 0x1D
#define CTRL_BREAK      157 // 0x9D
#define ALT_INDEX       56 // 0x38
#define ALT_BREAK       184 // 0xB8
#define LSHIFT_INDEX    42 // 0x2A
#define RSHIFT_INDEX    54 // 0x36
#define LSHIFT_BREAK    170 // 0xAA
#define RSHIFT_BREAK    182 // 0xB6
#define TAB_INDEX       15 // 0xF

#define ENTER_INDEX     28 // 0x1C
#define SPACE_INDEX     57 // 0x39

#define F1_INDEX        59 // 0x3B
#define F2_INDEX        60 // 0x3C
#define F3_INDEX        61 // 0x3D

#define MAX_BUF_SIZE    128

#define KEYBOARD_DATA       0x60 // port to get scan code from keyboard


int good_index; // within bounds of characters to type

int CAPS_CHECK; // capslock toggle

int SHIFT_CHECK; // shift pressed

int CTRL_CHECK; // control pressed

int ALT_CHECK; // alt pressed

int TAB_CHECK; // tab was pressed

int terminal_can_read; // terminal read allowed

char keyboard_buffer[MAX_BUF_SIZE]; // keyboard buffer size 128

//char old_buffer[MAX_BUF_SIZE]; // keyboard buffer size 128

int buffer_position; // keyboard buffer index


/* handles printing key pressed to screen */
extern void keyboard_handler();


/* initialize keyboard by enabling interrupts IRQ1 on pic */
extern void keyboard_init();

