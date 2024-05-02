/* terminal.c - Functions for terminal
 * vim:ts=4 noexpandtab
 */

/* ;
 * Description:
 * Inputs: 
 * Return Value: 
 */

#include "terminal.h"
#include "keyboard.h"
#include "syscalls.h"
#include "paging.h"
#include "file.h"
#include "i8259.h"
#include "schedule_switch.h"
#include "x86_desc.h"

#include "lib.h"

extern process_control_block_t* current_process;
extern int32_t current_pid;            // initial pid = 0
extern int32_t current_parent_pid;
int32_t boot_flag = -1;

/* int32_T terminal_init();
 * Description: intializes the three terminals, and boots terminal 3 as the scheduled
                terminal
 * Inputs: none
 * Return Value: 0
 */
int32_t terminal_init(){
    int i, j;
    add_vid_mem_storage();
    for (i = 0; i < 3; i++){
        if (i == 2){
            terminal_array[i].active = 0;
            terminal_array[i].scheduled = 1;
            terminal_array[i].shell_exists = 0;
        }
        else{
            terminal_array[i].active = 0;
            terminal_array[i].scheduled = 0;
            terminal_array[i].shell_exists = 0;
        }
        //execute((uint8_t*)"shell");
        terminal_array[i].screen_x = 0;
        terminal_array[i].screen_y = 0;
        terminal_array[i].buffer_position = 0;
        terminal_array[i].old_buffer_position = 0;
        terminal_array[i].terminal_can_read = 0;
        terminal_array[i].cur_term_pid = 0;
        terminal_array[i].opened_before = 0;
        //keyboard buff
        for (j = 0; j < 128; j++){
            terminal_array[i].keyboard_buffer[j] = '\0';
            terminal_array[i].old_buffer[j] = '\0';
        }
    }
    boot_flag = 0;
    terminal_array[0].active = 1;
    terminal_array[0].opened_before = 1;

    return 0;

}

/* int32_t terminal_switch(int32_t terminal_num);
 * Description: switches terminals when alt+f1/2/3 is pressed
 * Inputs: the terminal we want to switch to
 * Return Value: 0 on success or the same terminal, -1 on invalid input
 */
int32_t terminal_switch(int32_t terminal_num){
    /* 1. save current video memory to the proper background buffer for the terminal */
    /* 2. put new terminal into video memory from its stored background buffer */
    /* 3. switch to new terminal's program - create a shell for first time */
    cli();
    int i;
    uint32_t saved_pf_addr;

    if (terminal_num > 3 || terminal_num < 1){
        return -1;
    }

    //if already in terminal don't swap
    if ((terminal_num-1) == get_active_term()){
        sti();
        //send_eoi(1);
        return 0;
    }

    /* check if terminal already running - i.e. shell already exists */

    /* if not running, execute shell in that terminal */

    /* if it is running, save current and restore the terminal you switch you */

    /* even if not running, save current - pcb stuff? */
    int old_term_num = get_active_term();
    
    saved_pf_addr = page_table[VIDEO_MEMORY].pf_addr;
    page_table[VIDEO_MEMORY].pf_addr = ((uint32_t) OFFSET_VID_MEM_START) >> 12; // set b8000 virtual to b8000 physical
    flush_tlb();
    

    /* save the old screen  to corresponding background buffer*/
    /* b8000 -> 1 MB + old_terminal_num * 4 KB*/
    save_vid_mem(old_term_num);


    /* set new active terminal */
    for (i = 0; i < 3; i++){
        terminal_array[i].active = ((((0x4) >> (terminal_num-1)) >> (2-i)) & 0x1);  // even easier logic?? :) come back when we actually have nothing to do
    }

    /* paging stuff */

    swap_vid_mem(terminal_num);


    page_table[VIDEO_MEMORY].pf_addr = saved_pf_addr;
    flush_tlb();

    
    /* if no shell, initialize with shell */
    sti();

    return 0;
    

}

/* returns the index of the terminal with scheduled attribute equal to 1 */
/* 0 success, -1 fail */
int32_t get_scheduled_term_idx(){
    int i;
    for(i = 0; i < 3; i++) {
        if(terminal_array[i].scheduled == 1) {
            return i;
        }
    }
    return -1;
}

/* implements round-robin scheduling, making the next scheduled terminal to be cyclically chosen in the array */
/* 0 success, -1 fail */
int32_t set_next_scheduled() {
    int cur_scheduled_idx = get_scheduled_term_idx();
    if(cur_scheduled_idx != -1) {
        terminal_array[cur_scheduled_idx].scheduled = 0;
        terminal_array[(cur_scheduled_idx + 1) % 3].scheduled = 1;
        return 0;
    }
    return -1;
}

/* int32_t get_active_term();
 * Description: checks each terminal in the terminal array's active flag
                and returns thye index of the terminal if the flag is 1
 * Inputs: none
 * Return Value: index of terminal, otherwise -1 (on failure)
 */
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

    int32_t term_num = get_scheduled_term_idx();

    /* change to char* */
    char* buffer = buf;

    if (buf == NULL){

        return -1; // or should it be -1?
    }

    if (nbytes > MAX_BUF_SIZE){
        nbytes = MAX_BUF_SIZE;
    }

    while(terminal_array[term_num].terminal_can_read == 0){}; //wait until enter pressed
    for(i = 0; i < 128; i++){
            terminal_array[term_num].old_buffer[i] = '\0';
    }
    for(i = 0; i < nbytes; i++){

        if (i > (MAX_BUF_SIZE - 1)){
            terminal_array[term_num].terminal_can_read = 0;
            return i;
        }

        if (terminal_array[term_num].keyboard_buffer[i] != '\n'){
            buffer[i] = terminal_array[term_num].keyboard_buffer[i];
            terminal_array[term_num].old_buffer[i] = terminal_array[term_num].keyboard_buffer[i];
            terminal_array[term_num].keyboard_buffer[i] = '\0'; // clear keboard_buffer after a read
            terminal_array[term_num].terminal_can_read = 0;
        }
        else{
            buffer[i] = terminal_array[term_num].keyboard_buffer[i]; // returns when key buffer is enter
            terminal_array[term_num].keyboard_buffer[i] = '\0'; // clear keboard_buffer after a read
            terminal_array[term_num].terminal_can_read = 0;
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
    cli();
    int i = 0;

    /* change to char* */
    const char* buffer = buf;
    
    if (buf == NULL){
        sti();
        return -1; // or should it be -1?
    }

    if (nbytes > 1024){
        nbytes = 1024;
    }

    //if active & schedule then putc_key (to active term)
    //if not active & schedules then putc_term
    //int32_t term = get_active_term();

            
    for (i = 0; i < nbytes; i++){
        if ((buffer[i] != '\0')){ // only prints characters
            putc_term(buffer[i]);
        }
    }
    sti();
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

