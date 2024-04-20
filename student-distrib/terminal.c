/* terminal.c - Functions for terminal
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "keyboard.h"
#include "syscalls.h"

#include "lib.h"


int32_t terminal_init(){
    int i, j;

    for (i = 0; i < 3; i++){
        if (i == 0){
            terminal_array[i].active = 1;
            terminal_array[i].shell_exists = 1;
        }
        else{
            terminal_array[i].active = 0;
            terminal_array[i].shell_exists = 0;
        }
        terminal_array[i].screen_x = 0;
        terminal_array[i].screen_y = 0;
        terminal_array[i].buffer_position = 0;
        terminal_array[i].terminal_can_read = 0;
        //keyboard buff
        for (j = 0; j < 128; j++){
            terminal_array[i].keyboard_buffer[j] = '\0';
        }
    }
    return 0;

}


int32_t terminal_switch(int32_t terminal_num){
    int i;
    /* wht the fuck do i do here */
    // if (terminal_number == 1){
    //     terminal_array[0].active = 1;
    //     terminal_array[1].active = 0;
    //     terminal_array[2].active = 0;
    // }
    // else if (terminal_number == 2){
    //     terminal_array[0].active = 0;
    //     terminal_array[1].active = 1;
    //     terminal_array[2].active = 0;
    // }
    // else if (terminal_number == 3){
    //     terminal_array[0].active = 0;
    //     terminal_array[1].active = 0;
    //     terminal_array[2].active = 1;
    // }
    if (terminal_num > 3 || terminal_num < 1){
        return -1;
    }

    /* check if terminal already running - i.e. shell already exists */

    /* if not running, execute shell in that terminal */

    /* if it is running, save current and restore the terminal you switch you */

    /* even if not running, save current - pcb stuff? */

    for (i = 0; i < 3; i++){
        terminal_array[i].active = ((((0x4) >> (terminal_num-1)) >> (2-i)) & 0x1);  // even easier logic?? :) come back when we actually have nothing to do
    }

    /* if no shell, initialize with shell */
    /*
    if (terminal_array[terminal_num-1].shell_exists == 0){ 
        execute((uint8_t*)"shell");
    }*/

    printf("%d\n", terminal_num);


    return 0;
    

}

int32_t get_active_term(){
    int i;
    /* find the active terminal */
    for(i=0; i < 3; i++){
        if (terminal_array[i].active){
            return i;
        }
    }
    return -1;
}

/* terminal_open
 *  Functionality: Open terminal
 *  Arguments: None
 *  Return: None
 */
int32_t terminal_open(const uint8_t* filename){
    return 0;
}


/* terminal_read
 *  Functionality: Reads from the keyboard buffer once enter was pressed.
 *  Arguments: fd: file directory (not used right now)
 *             buf: buffer to read to
 *             nbytes: number of bytes to read 
 *  Return: number of bytes read
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){

    int i = 0;

    int32_t term_num = get_active_term();

    /* change to char* */
    char* buffer = buf;

    if (buf == NULL){
        return -1; // or should it be -1?
    }

    if (nbytes > MAX_BUF_SIZE){
        nbytes = MAX_BUF_SIZE;
    }

    while(terminal_array[term_num].terminal_can_read == 0){}; //wait until enter pressed

    for(i = 0; i < nbytes; i++){

        if (i > (MAX_BUF_SIZE - 1)){
            terminal_array[term_num].terminal_can_read = 0;
            return i;
        }

        if (terminal_array[term_num].keyboard_buffer[i] != '\n'){
            buffer[i] = terminal_array[term_num].keyboard_buffer[i];
            //old_buffer[i] = keyboard_buffer[i];
            terminal_array[term_num].keyboard_buffer[i] = '\0'; // clear keboard_buffer after a read
            terminal_array[term_num].terminal_can_read = 0;
            //printf("cleared");
        }
        else{
            buffer[i] = terminal_array[term_num].keyboard_buffer[i]; // returns when key buffer is enter
            terminal_array[term_num].keyboard_buffer[i] = '\0'; // clear keboard_buffer after a read
            //printf("cleared2");
            terminal_array[term_num].terminal_can_read = 0;
            //printf("%d\n", i);
            return i+1;
        }
    }

    terminal_array[term_num].terminal_can_read = 0;
    return nbytes;

}

/* terminal_write
 *  Functionality: Print buffer to the terminal screen.
 *  Arguments: fd: file directory (not used right now)
 *             buf: buffer used to output on screen
 *             nbytes: number of bytes to write 
 *  Return: number of bytes written
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    
    int i = 0;

    /* change to char* */
    const char* buffer = buf;

    if (buf == NULL){
        return -1; // or should it be -1?
    }

    if (nbytes > 1024){
        nbytes = 1024;
    }
    for (i = 0; i < nbytes; i++){
        if ((buffer[i] != '\0')){ // only prints characters
            putc(buffer[i]);
        }
        //else if (buffer[i] == '\n'){ // returns when end of buffer (the new line)
        //    putc(buffer[i]);
        //    //return i+1;
        //}
        //else if (buffer[i] == '\0'){
        //     putc('\n');
        //     return i;
        // }
    }

    return nbytes;

}

/* terminal_close
 *  Functionality: Close terminal
 *  Arguments: None
 *  Return: None
 */
int32_t terminal_close(int32_t fd){
    return -1;
}

