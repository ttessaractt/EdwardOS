/* syscall_helper.c - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "syscall_helper.h"

#include "lib.h"
#include "file.h"
#include "paging.h"
#include "loader.h"

current_pid = 0; // initial pid = 0
current_parent_pid = 0;

int32_t execute_help(unsigned char* command){

    if (command == NULL){
        return -1;
    }

    unsigned char file_name[32+1]; // 32+1?
    unsigned char arguments[128]; // check lengths?

    // PARSE ARGS
    /* populates file_name */
    parse_arguments(command, file_name, arguments);

    // CHECK FILE VALIDITY
    int32_t entry_addr = check_file_validity(file_name);
    /* not an executable file */
    if(entry_addr == -1) {
        return -1;
    }

    /* entry_addr now stored in entry_addr */

    // SET UP PAGING
    // allocates memory for tasks 
    allocate_tasks(1);

    // LOAD FILE INTO MEMORY
    program_loader(file_name, 1);

    // CREATE PCB 


    // CONTEXT SWITCH AND IRET
    

    return -1;

}


int32_t parse_arguments(unsigned char* buf, unsigned char* file_name, unsigned char* arguments){

    /* return -1 for fail */
    if (buf == NULL){
        return -1;
    }

    /* filter out spaces from first file name */
    int cur_idx = 0;
    while(buf[cur_idx] == 0x20) {
        cur_idx++;
    }

    /* at first file name */
    int i = 0;
    while(buf[cur_idx] != 0x20) {
        file_name[i] = buf[cur_idx];
        i++;
        cur_idx++;
    }
    file_name[i] = '\0';
    
    /* filter out spaces between first file name and next */
    while(buf[cur_idx] == 0x20) {
        cur_idx++;
    }

    /* copy to arguments everything past first file name + spaces */
    int j;
    int l = 0;
    for(j = cur_idx; j < strlen(buf); j++) {
        arguments[l] = buf[j];
        l++;
    }
    arguments[l] = '\0';

    /* testing */
    printf("\n");
    printf("%s", file_name);
    printf("\n");
    printf("%s", arguments);

    return 1; //success

}

int32_t initialize_pcb(unsigned char* file_name){

    current_pid++;
    process_control_block_t * pcb_new = 0x800000 - (0x2000 * current_pid); //(should be 0x800000 - PID * x)

    pcb_new->pid = current_pid; // becomes 1
    pcb_new->parent_pid = current_parent_pid; // 0 - no parent yet

    pcb_new->tss_esp0 = 0x800000 - (0x2000 * (current_pid-1));
    

    /* initialzie file array */

    return -1;

} 
