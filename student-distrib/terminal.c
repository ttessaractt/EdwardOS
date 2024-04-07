/* terminal.c - Functions for terminal
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "keyboard.h"

#include "lib.h"


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

    /* change to char* */
    char* buffer = buf;

    if (buf == NULL){
        return -1; // or should it be -1?
    }
    printf("terminal read 38\n");
    while(terminal_can_read == 0){}; //wait until enter pressed

    for(i = 0; i < nbytes; i++){

        if (i > (MAX_BUF_SIZE - 1)){
            terminal_can_read = 0;
            return i;
        }

        if (keyboard_buffer[i] != '\n'){
            buffer[i] = keyboard_buffer[i];
            keyboard_buffer[i] = '\0'; // clear keboard_buffer after a read
            terminal_can_read = 0;
            //printf("cleared");
        }
        else{
            buffer[i] = keyboard_buffer[i]; // returns when key buffer is enter
            keyboard_buffer[i] = '\0'; // clear keboard_buffer after a read
            //printf("cleared2");
            terminal_can_read = 0;
            terminal_write(fd, buffer, nbytes); // tests that buffer has ls in it
            printf("%d\n", i); // prints 2 for 2 bytes
            printf("%c\n", buffer[i-1]); // prints last value in buffer, should be \n
            return i;
        }
    }

    terminal_can_read = 0;
    terminal_write(fd, buffer, nbytes);
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

    if (nbytes > MAX_BUF_SIZE){
        nbytes = MAX_BUF_SIZE;
    }
    for (i = 0; i < nbytes; i++){
        if ((buffer[i] != '\n') && (buffer[i] != '\0')){ // only prints characters
            putc(buffer[i]);
        }
        else if (buffer[i] == '\n'){ // returns when end of buffer (the new line)
            putc(buffer[i]);
            return i;
        }
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
