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
    // warning: don't try to print just a space
    static char lookup[] = "..1234567890-=..qwertyuiop[]..asdfghjkl;\'`.\\zxcvbnm,./"; // lookup table of characters based on keyboard scan code
    static char lookup_cap[] = "..1234567890-=..QWERTYUIOP[]..ASDFGHJKL;\'`.\\ZXCVBNM,./"; // lookup table for capital letters
    static char lookup_shift[] = "..!@#$%^&*()_+..QWERTYUIOP{}..ASDFGHJKL:\"~.|ZXCVBNM<>?"; // lookup table for shift letters
    static char lookup_cap_shift[] = "..!@#$%^&*()_+..qwertyuiop{}..asdfghjkl:\"~.|zxcvbnm<>?"; // lookup table for caps then shift letters
    
    char p;

    uint32_t key = inb(KEYBOARD_DATA); // scan code from port x60

    //printf("%x", key);
    // 3A = caps
    if (key == CAPS_INDEX){
        CAPS_CHECK = !CAPS_CHECK;
        //printf("%d", CAPS);
    }

    // 2A = L shift, 36 = R shift
    if ((key == 0x2A) || (key == 0x36)){
        SHIFT_CHECK = 1;
        //printf("shift ");
    }
    else if ((key == 0xAA) || (key == 0xB6)){ // xAA, xB6 is break
        SHIFT_CHECK = 0;
        //printf("shift off ");
    }

    if (CAPS_CHECK){
    // checks if key is within bounds of numbers and lower case letters
        if (SHIFT_CHECK){
        // checks if key is within bounds of numbers and lower case letters
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_cap_shift[key];
                printf("%c", p);
            }
        }
        else{
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_cap[key];
                printf("%c", p);
            }
        }
    }
    else{
        if (SHIFT_CHECK){
        // checks if key is within bounds of numbers and lower case letters
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_shift[key];
                printf("%c", p);
            }
        }
        else{
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup[key];
                printf("%c", p);
            }
        }
    }

    
    
    send_eoi(1); //sends end of interrupt on IRQ1
    
};

