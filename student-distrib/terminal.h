/* terminal.h - Functions for terminal
 * vim:ts=4 noexpandtab
 */

#include "types.h"

/*holder for PCB information*/
typedef struct terminal_t{
    /* input buffer */
    char keyboard_buffer[128]; // keyboard buffer size 128

    char old_buffer[128*5];

    int32_t old_buffer_position;

    int32_t buffer_position;

    int32_t terminal_can_read;

    /* screen position */
    int32_t screen_x;
    int32_t screen_y;

    int32_t cur_term_pid;

    /* 1 is active, 0 is not active */
    int32_t active;

    /* 1 means currently scheduled, 0 means not scheduled */
    /* only one terminal can be scheduled at a time*/
    int32_t scheduled;

    int32_t shell_exists;

    int32_t opened_before;

} terminal_t; 

terminal_t terminal_array[3];
int32_t boot_flag;

// terminal initialize
int32_t terminal_init();

// terminal switch
int32_t terminal_switch(int32_t terminal_num);

/* get the index of the currently scheduled terminal */
int32_t get_scheduled_term_idx();

// sets next terminal to be scheduled
int32_t set_next_scheduled();

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
