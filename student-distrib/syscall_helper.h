/* syscall_helper.h - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "types.h"

typedef struct process_control_block_t{
    int32_t pid;
    int32_t parent_pid;

    uint32_t esp;
    uint32_t tss_esp0;

    struct process_control_block_t * next;
    struct process_control_block_t * parent;

    /* need file discriptor array */
    //file_info file_d_array[length]; //need to make it!!

    //signal - add later

} process_control_block_t; 

process_control_block_t PCB_1;
process_control_block_t PCB_2;

int32_t execute_help(unsigned char* command);

/* need to pass in file name and argument buffer of proper size */
int32_t parse_arguments(unsigned char* buf, unsigned char* file_name, unsigned char* arguments);


