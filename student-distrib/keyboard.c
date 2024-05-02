/* keyboard.c - Functions to initialize keyboard to work with 8259 and print keys to screen
 * vim:ts=4 noexpandtab
 */


#include "keyboard.h"
#include "i8259.h"
#include "terminal.h"
#include "file.h"

#include "lib.h"

int terminal_can_read = 0;
int counter = 0;

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
    static char names[16][33] = {"sigtest", "shell", "grep", "syserr", "rtc", "fish", "counter", "pingpong", "cat", "frame0.txt", 
    "verylargetextwithverylongname.tx", "created.txt", "testprint", "frame1.txt", "hello"};
    char space = ' ';

    char p;
    
    int k;
    int j;

    good_index = 0;

    /* get active terminal index */
    int32_t term_num = get_active_term();

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

    if ((key == ALT_INDEX)){ // x38 = L and R control 
        ALT_CHECK = 1;
        //printf("alt pressed ");
    }
    else if ((key == ALT_BREAK)){ // xB8 break
        ALT_CHECK = 0;
        //printf("alt let go ");
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

    int32_t old_length = strlen(terminal_array[term_num].old_buffer);

    if (key == 72){
        for(k = 0; k < old_length; k++){
            if (terminal_array[term_num].buffer_position > 127){
                break;
            }
            terminal_array[term_num].keyboard_buffer[k] = terminal_array[term_num].old_buffer[k];
            terminal_array[term_num].old_buffer[k] = '\0';
            putc_key(terminal_array[term_num].keyboard_buffer[k]);
            terminal_array[term_num].buffer_position++;
        }
        
    }

    if (CTRL_CHECK){
        // need to clear screen if press L
        if (key == L_INDEX){
            clear_screen_term();
            send_eoi(1);
            return;
        }

    }

    if (ALT_CHECK){
        // need to clear screen if press L
        if (key == F1_INDEX){
            terminal_switch(1);
            send_eoi(1);
            return;
        }
        else if (key == F2_INDEX){
            terminal_switch(2);
            send_eoi(1);
            return;
        }
        else if (key == F3_INDEX){
            terminal_switch(3);
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
                    if (terminal_array[term_num].buffer_position > 0){
                        removec_key(p);
                        terminal_array[term_num].buffer_position--;
                    }
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
                    if (terminal_array[term_num].buffer_position > 0){
                        removec_key(p);
                        terminal_array[term_num].buffer_position--;
                    }
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
                    if (terminal_array[term_num].buffer_position > 0){
                        removec_key(p);
                        terminal_array[term_num].buffer_position--;
                    }
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
                    if (terminal_array[term_num].buffer_position > 0){
                        removec_key(p);
                        terminal_array[term_num].buffer_position--;
                    }
            }
        }
    }

    if (good_index){ // if key was within bounds, allow to continue

        if (TAB_CHECK){ // if a tab
            int i;

            int skiparoo = 0;
            //int double_break = 0;

            char buf_check[2];
            unsigned char fake_file_name[32+1]; 
            unsigned char file_name[32+1];
            unsigned char arguments[128]; 

            parse_arguments((unsigned char*)terminal_array[term_num].keyboard_buffer, fake_file_name, arguments);

            if (arguments[0] != '\0'){
                unsigned char fake_arguments[128];
                for(i = 0; i < 128; i++){
                    fake_arguments[i] = arguments[i];
                }
                parse_arguments(fake_arguments, fake_file_name, arguments);   
            }
            for(i = 0; i < 32; i++){
                    file_name[i] = fake_file_name[i];
            }

            buf_check[0] = terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position-2];
            buf_check[1] = terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position-1];

            if((terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position-1] != ' ' 
            && terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position-2] != ' ' ) && (terminal_array[term_num].buffer_position != 0)) {
                for(i = 0; i < 16; i++) {
                    //int k;
                    int name_length = strlen(names[i]);
                    int file_length = strlen((char*)file_name);
                    //for(k = 0; k < strlen(names[i]) - 1; k++) {
                        if(strncmp(names[i], (char*)file_name, file_length) == 0) {
                            
                            int j;
                            //double_break = 1;
                            if(terminal_array[term_num].buffer_position + name_length > 127) {
                                break;
                            } else {
                                for(j = file_length; j < name_length; j++) {
                                    terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position] = names[i][j];
                                    putc_key(names[i][j]);
                                    terminal_array[term_num].buffer_position++;
                                }

                                skiparoo = 1;
                                break;
                            }
                        }
    
                    //if(double_break) {
                    //    double_break = 0;
                    //    break;
                    //}
                }
            }

            if(skiparoo == 0) {
                for (j = 0; j < 4; j++){ // print space 4 times for a tab
                    if ((terminal_array[term_num].buffer_position == (MAX_BUF_SIZE - 1)) && (p != '\n')){ // max size
                        send_eoi(1);
                        return; 
                    }

                    terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position] = p;
                    //while(!(terminal_array[term_num].scheduled)){}; 
                    putc_key(terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position]); // prints key
                    terminal_array[term_num].buffer_position++;
            
                    if (p == '\n'){ // if pressed enter
                        terminal_array[term_num].terminal_can_read = 1; // allow terminal to read
                        terminal_array[term_num].buffer_position = 0; // reset buffer position to 0    
                    }
                    else{
                        terminal_array[term_num].terminal_can_read = 0;
                    }

                }
            }
            skiparoo = 0;
            
            
        }
        else{ // not a tab
            if ((terminal_array[term_num].buffer_position == (MAX_BUF_SIZE - 1)) && (p != '\n')){ // max size
                send_eoi(1);
                return; 
            }

            terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position] = p;
            //while(!(terminal_array[term_num].scheduled)){}; 
            putc_key(terminal_array[term_num].keyboard_buffer[terminal_array[term_num].buffer_position]); // prints key
            terminal_array[term_num].buffer_position++;
        
            if (p == '\n'){ // if pressed enter
                terminal_array[term_num].terminal_can_read = 1; // allow terminal to read
                terminal_array[term_num].buffer_position = 0; // reset buffer position to 0   
            }
            else{
                    terminal_array[term_num].terminal_can_read = 0;
                }
        }
    }
   
    send_eoi(1); //sends end of interrupt on IRQ1
   
};


