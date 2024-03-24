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
    // warning: don't try to print just a space
    static char lookup[] = "..1234567890-=..qwertyuiop[]..asdfghjkl;\'`.\\zxcvbnm,./"; // lookup table of characters based on keyboard scan code
    static char lookup_cap[] = "..1234567890-=..QWERTYUIOP[]..ASDFGHJKL;\'`.\\ZXCVBNM,./"; // lookup table for capital letters
    static char lookup_shift[] = "..!@#$%^&*()_+..QWERTYUIOP{}..ASDFGHJKL:\"~.|ZXCVBNM<>?"; // lookup table for shift letters
    static char lookup_cap_shift[] = "..!@#$%^&*()_+..qwertyuiop{}..asdfghjkl:\"~.|zxcvbnm<>?"; // lookup table for caps then shift letters
    // add in space like ' '
    // need to add control
    char space = ' ';

    char p;

    int j;

    good_index = 0;

    uint32_t key = inb(KEYBOARD_DATA); // scan code from port x60


    //printf("%x", key);
    // 3A = caps
    if (key == CAPS_INDEX){
        CAPS_CHECK = !CAPS_CHECK;
        //printf("%d", CAPS);
    }

    if ((key == 0x1D)){ // x1D = L and R control, 
        CTRL_CHECK = 1;
        //printf("control ");
    }
    else if ((key == 0x9D)){ // x9D
        CTRL_CHECK = 0;
        //printf("control off ");
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

    if ((key == 0xF)){
        tab_c = 1;
        //printf("shift ");
    }
    else{ // xAA, xB6 is break
        tab_c = 0;
        //printf("shift off ");
    }
   /*  if(key == BACKSP_INDEX){
        // call some function?
        good_index = 1;
        BACKSP_CHECK = 1;
        removec(p); // change from p
        keyboard_buffer[buffer_postion] = 'P';
    }
    else if(key == 0x8E){
        BACKSP_CHECK = 0;
    } */


    if (CTRL_CHECK){
        // need to clear screen if press L
        if (key == 0x26){
            clear_screen();
            send_eoi(1);
            return;
        }

    }

    if (CAPS_CHECK){
    // checks if key is within bounds of numbers and lower case letters
        if (SHIFT_CHECK){
        // checks if key is within bounds of numbers and lower case letters
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_cap_shift[key];
                //printf("%c", p); 
                good_index = 1;
            }
            else if(key == 28){
                p = '\n';
                good_index = 1;
            }
            else if(key == 57){
                p = space;
                good_index = 1;
            }
            else if(key == 15){
                p = space;
                good_index = 1;
                //tab_c = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_postion--;
                    //keyboard_buffer[buffer_postion] = 'P';
            }
        }
        else{
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_cap[key];
                //printf("%c", p);
                good_index = 1;
            }
            else if(key == 28){
                p = '\n';
                good_index = 1;
            }
            else if(key == 57){
                p = space;
                good_index = 1;
            }
            else if(key == 15){
                p = space;
                good_index = 1;
                //tab_c = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_postion--;
                    //keyboard_buffer[buffer_postion] = 'P';
            }
        }
    }
    else{
        if (SHIFT_CHECK){
        // checks if key is within bounds of numbers and lower case letters
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup_shift[key];
                //printf("%c", p);
                good_index = 1;
            }
            else if(key == 28){
                p = '\n';
                good_index = 1;
            }
            else if(key == 57){
                p = space;
                good_index = 1;
            }
            else if(key == 15){
                p = space;
                good_index = 1;
                //tab_c = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_postion--;
                    //keyboard_buffer[buffer_postion] = 'P';
            }
        }
        else{
            if (((ONE_INDEX <= key) && (key <= EQUALS_INDEX)) | ((Q_INDEX <= key) && (key <= BRACKET_INDEX)) | ((A_INDEX <= key) && (key <= TILDE_INDEX)) | ((BSLASH_INDEX <= key) && (key <= QUESTION_INDEX))){
                p = lookup[key];
                //printf("%c", p);
                good_index = 1;
            }
            else if(key == 28){
                p = '\n';
                good_index = 1;
            }
            else if(key == 57){
                p = space;
                good_index = 1;
            }
            else if(key == 15){
                p = space;
                good_index = 1;
                //tab_c = 1;
            }
            else if(key == BACKSP_INDEX){
                    removec(p);
                    buffer_postion--;
                    //keyboard_buffer[buffer_postion] = 'P';
            }
        }
    }

    if (good_index){

        if (tab_c){
            for (j = 0; j < 4; j++){
                if ((buffer_postion == 127) && (p == '\n')){ // max size
                    send_eoi(1);
                    return; 
                }
                printf("%c", p);
                keyboard_buffer[buffer_postion] = p;
                buffer_postion++;
        
                if (p == '\n'){
                    //printf("enter!!");
                    terminal_can_read = 1;
                    buffer_postion = 0;    
                }

            }
            
        }
        else{
            if ((buffer_postion == 127) && (p == '\n')){ // max size
                send_eoi(1);
                return; 
            }
            printf("%c", p);
            keyboard_buffer[buffer_postion] = p;
            buffer_postion++;
        
            if (p == '\n'){
                //printf("enter!!");
                terminal_can_read = 1;
                buffer_postion = 0;    
            }
        }
    }
   
    send_eoi(1); //sends end of interrupt on IRQ1
   
};

// still need terminal open close

int32_t terminal_key_read(int32_t fd, char* buf, int32_t nbytes){
    int i = 0;
    while(terminal_can_read == 0){}; //wait

    for(i = 0; i < nbytes; i++){

        if (i > 127){
            return nbytes;
        }

        if (keyboard_buffer[i] != '\n'){
            buf[i] = keyboard_buffer[i];
        }
        else{
            buf[i] = keyboard_buffer[i];
            return nbytes;
        }
    }

    return nbytes;
}

int32_t terminal_key_write(int32_t fd, char* buf, int32_t nbytes){
    int i = 0;
    for (i = 0; i < nbytes; i++){
        putc(buf[i]);
    }

    return nbytes;


}
