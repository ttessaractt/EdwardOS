/* syscall_helper.h - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "types.h"

typedef struct process_control_block_t{
    int32_t pid;
    int32_t parent_pid;

    uint32_t esp;
    uint32_t tss_esp0;

    /* need file discriptor array */
    //file_array_t file_d_array; //need to make it!!

} process_control_block_t; 



int32_t execute(char* command);

/* need to pass in file name and argument buffer of proper size */
int32_t parse_arguments(char* buf, char* file_name, char* arguments);


