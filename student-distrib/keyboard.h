/* keyboard.h - Functions to initialize keyboard to work with 8259 and print keys to screen
 * vim:ts=4 noexpandtab
 */

// scan codes for numbers 0 to 9 and letters a to z

#define ONE_INDEX       2
#define ZERO_INDEX      11

#define Q_INDEX         16
#define P_INDEX         25

#define A_INDEX         30
#define L_INDEX         38

#define Z_INDEX         44
#define M_INDEX         50

#define KEYBOARD_DATA       0x60 // port to get scan code from keyboard

/* handles printing key pressed to screen */
extern void keyboard_handler();

/* initialize keyboard by enabling interrupts IRQ1 on pic */
extern void keyboard_init();
