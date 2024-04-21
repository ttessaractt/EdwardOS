/* terminal.h - Functions for terminal
 * vim:ts=4 noexpandtab
 */

#include "types.h"

/*holder for PCB information*/
typedef struct terminal_t{
    /* input buffer */
    char keyboard_buffer[128]; // keyboard buffer size 128

    int32_t buffer_position;

    int32_t terminal_can_read;

    /* screen position */
    int32_t screen_x;
    int32_t screen_y;

    int32_t cur_term_pid;

    /* 1 is active, 0 is not active */
    int32_t active;

    int32_t shell_exists;

} terminal_t; 

terminal_t terminal_array[3];

// terminal initialize
int32_t terminal_init();

// terminal switch
int32_t terminal_switch(int32_t terminal_num);

// returns active terminal index
int32_t get_active_term();

// terminal_open
int32_t terminal_open(const uint8_t* filename);

// terminal_read
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

// terminal_write
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

// terminal_close
int32_t terminal_close(int32_t fd);
