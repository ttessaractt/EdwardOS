/* keyboard.c - Functions to initialize keyboard to work with 8259 and print keys to screen
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"

/* keyboard_init
 *  Functionality: enables interrupt on IRQ1 on PIC for keyboard functionality
 *  Arguments: none
 *  Return: none
 */
void keyboard_init(){
    enable_irq(1); // enables interrupt on IRQ1 on PIC
};

/* keyboard_handler
 *  Functionality: when key is pressed, prints respective key to the screen
 *                 for characters 0-9 and A-Z
 *  Arguments: none
 *  Return: none
 */
void keyboard_handler(){

    static char lookup[] = "..1234567890....qwertyuiop....asdfghjkl.....zxcvbnm"; // lookup table of characters based on keyboard scan code

    char p;

    uint32_t key = inb(KEYBOARD_DATA); // scan code from port x60

    // checks if key is within bounds of numbers and lower case letters
    if (((ONE_INDEX <= key) && (key<= ZERO_INDEX)) | ((Q_INDEX <= key) && (key <= P_INDEX)) | ((A_INDEX <= key) && (key <= L_INDEX)) | ((Z_INDEX <= key) && (key <= M_INDEX))){
        p = lookup[key];
        printf("%c", p);
    }
    
    send_eoi(1); //sends end of interrupt on IRQ1
    
};

