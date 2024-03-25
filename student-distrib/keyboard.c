/* keyboard.c - Functions to initialize keyboard to work with 8259 and print keys to screen
 * vim:ts=4 noexpandtab
 */


#include "keyboard.h"
#include "i8259.h"

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

    /* lookup tables for all characters */
    static char lookup[] = "..1234567890-=..qwertyuiop[]..asdfghjkl;\'`.\\zxcvbnm,./"; // lookup table of characters based on keyboard scan code
    static char lookup_cap[] = "..1234567890-=..QWERTYUIOP[]..ASDFGHJKL;\'`.\\ZXCVBNM,./"; // lookup table for capital letters
    static char lookup_shift[] = "..!@#$%^&*()_+..QWERTYUIOP{}..ASDFGHJKL:\"~.|ZXCVBNM<>?"; // lookup table for shift letters
    static char lookup_cap_shift[] = "..!@#$%^&*()_+..qwertyuiop{}..asdfghjkl:\"~.|zxcvbnm<>?"; // lookup table for caps then shift letters

    char space = ' ';

    char p;

    int j;

    good_index = 0;

    uint32_t key = inb(KEYBOARD_DATA); // scan code from port x60

    
    if (key == CAPS_INDEX){ // x3A = caps
        CAPS_CHECK = !CAPS_CHECK;
    }

    if ((key == CTRL_INDEX)){ // x1D = L and R control 
        CTRL_CHECK = 1;
    }
    else if ((key == CTRL_BREAK)){ // x9D break
        CTRL_CHECK = 0;
    }

    
    if ((key == LSHIFT_INDEX) || (key == RSHIFT_INDEX)){ // x2A = L shift, x36 = R shift
        SHIFT_CHECK = 1;
    }
    else if ((key == LSHIFT_BREAK) || (key == RSHIFT_BREAK)){ // xAA, xB6 is break
        SHIFT_CHECK = 0;
    }

    if ((key == TAB_INDEX)){ // xF = tab
        TAB_CHECK = 1;
    }
    else{
        TAB_CHECK = 0;
    }


    if (CTRL_CHECK){
        // need to clear screen if press L
        if (key == 0x26){
            clear_screen();
            send_eoi(1);
            return;
        }

    }

    if (CAPS_CHECK){ // if capslock was toggled
        if (SHIFT_CHECK){ // if press shift in capslock
            // checks bounds of key
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_cap_shift[key];
                good_index = 1;
            }
            else if(key == ENTER_INDEX){
                p = '\n';
                good_index = 1;
            }
            else if(key == SPACE_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == TAB_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_position--;
            }
        }
        else{ // just capslock toggled
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_cap[key];
                good_index = 1;
            }
            else if(key == ENTER_INDEX){
                p = '\n';
                good_index = 1;
            }
            else if(key == SPACE_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == TAB_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_position--;
            }
        }
    }
    else{ // capslock not toggled
        if (SHIFT_CHECK){ // if shift pressed
        // checks if key is within bounds
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_shift[key];
                good_index = 1;
            }
            else if(key == ENTER_INDEX){
                p = '\n';
                good_index = 1;
            }
            else if(key == SPACE_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == TAB_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_position--;
            }
        }
        else{ // shift not pressed
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup[key];
                good_index = 1;
            }
            else if(key == ENTER_INDEX){
                p = '\n';
                good_index = 1;
            }
            else if(key == SPACE_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == TAB_INDEX){
                p = space;
                good_index = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_position--;
            }
        }
    }

    if (good_index){ // if key was within bounds, allow to continue

        if (TAB_CHECK){ // if a tab
            for (j = 0; j < 4; j++){ // print space 4 times for a tab
                if ((buffer_position == (MAX_BUF_SIZE - 1)) && (p != '\n')){ // max size
                    send_eoi(1);
                    return; 
                }

                keyboard_buffer[buffer_position] = p;
                putc(keyboard_buffer[buffer_position]); // prints key
                buffer_position++;
        
                if (p == '\n'){ // if pressed enter
                    terminal_can_read = 1; // allow terminal to read
                    buffer_position = 0; // reset buffer position to 0    
                }
                else{
                    terminal_can_read = 0;
                }

            }
            
        }
        else{ // not a tab
            if ((buffer_position == (MAX_BUF_SIZE - 1)) && (p != '\n')){ // max size
                send_eoi(1);
                return; 
            }

            keyboard_buffer[buffer_position] = p;
            putc(keyboard_buffer[buffer_position]); // prints key
            buffer_position++;
        
            if (p == '\n'){ // if pressed enter
                terminal_can_read = 1; // allow terminal to read
                buffer_position = 0; // reset buffer position to 0    
            }
            else{
                    terminal_can_read = 0;
                }
        }
    }
   
    send_eoi(1); //sends end of interrupt on IRQ1
   
};



/* terminal_key_open
 *  Functionality: Open terminal
 *  Arguments: None
 *  Return: None
 */
int32_t terminal_key_open(){
    return 0;
}


/* terminal_key_read
 *  Functionality: Reads from the keyboard buffer once enter was pressed.
 *  Arguments: fd: file directory (not used right now)
 *             buf: buffer to read to
 *             nbytes: number of bytes to read 
 *  Return: number of bytes read
 */
int32_t terminal_key_read(int32_t fd, char* buf, int32_t nbytes){

    int i = 0;

    while(terminal_can_read == 0){}; //wait until enter pressed

    for(i = 0; i < nbytes; i++){

        if (i > (MAX_BUF_SIZE - 1)){
            terminal_can_read = 0;
            return i;
        }

        if (keyboard_buffer[i] != '\n'){
            buf[i] = keyboard_buffer[i];
            keyboard_buffer[i] = '\0';
            terminal_can_read = 0;
            //printf("cleared");
        }
        else{
            buf[i] = keyboard_buffer[i]; // returns when key buffer is enter
            keyboard_buffer[i] = '\0';
            //printf("cleared2");
            terminal_can_read = 0;
            return i;
        }
    }

    terminal_can_read = 0;
    return nbytes;

}

/* terminal_key_write
 *  Functionality: Print buffer to the terminal screen.
 *  Arguments: fd: file directory (not used right now)
 *             buf: buffer used to output on screen
 *             nbytes: number of bytes to write 
 *  Return: number of bytes written
 */
int32_t terminal_key_write(int32_t fd, char* buf, int32_t nbytes){
    
    int i = 0;

    if (nbytes > MAX_BUF_SIZE){
        nbytes = MAX_BUF_SIZE;
    }
    for (i = 0; i < nbytes; i++){
        if ((buf[i] != '\n') && (buf[i] != '\0')){ // only prints characters
            putc(buf[i]);
        }
        else if (buf[i] == '\n'){ // returns when end of buffer (the new line)
            putc(buf[i]);
            return i;
        }
    }

    return nbytes;

}

/* terminal_key_close
 *  Functionality: Close terminal
 *  Arguments: None
 *  Return: None
 */
int32_t terminal_key_close(){
    return 0;
}
