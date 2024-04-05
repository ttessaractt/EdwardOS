/* syscall_helper.c - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "syscall_helper.h"

#include "lib.h"
#include "file.h"
#include "paging.h"
#include "loader.h"
#include "x86_desc.h"

int32_t current_pid = 0; // initial pid = 0
int32_t current_parent_pid = 0;
process_control_block_t* current_process = NULL;

int32_t execute_help(unsigned char* command){

    //cli();

    if (command == NULL){
        return -1;
    }

    unsigned char file_name[32+1]; // 32+1?
    unsigned char arguments[128]; // check lengths?

    // PARSE ARGS
    /* populates file_name */
    parse_arguments(command, file_name, arguments);

    // CHECK FILE VALIDITY
    int32_t entry_addr = check_file_validity((char*)file_name);
    /* not an executable file */
    if(entry_addr == -1) {
        return -1;
    }

    /* entry_addr now stored in entry_addr */
    current_parent_pid = current_pid;

    // CREATE PCB 
    initialize_pcb(file_name);

    // SET UP PAGING
    // allocates memory for tasks 
    allocate_tasks(current_process->pid);

    // LOAD FILE INTO MEMORY
    program_loader((char*)file_name, 1);

    

    // CONTEXT SWITCH AND IRET
    

    return -1;

}

int32_t halt_help(unsigned char* status){
    /* not done! */
    /* get the esp0 of the parent */    
    process_control_block_t* pcb_parent = (process_control_block_t*) 0x800000 - (0x2000 * (current_process->parent_pid));
    tss.esp0 = pcb_parent->tss_esp0;
    tss.ss0 = KERNEL_DS;

    current_pid = 0;
    /* the 8 bit input is BL (register) which should then be expanded */
    /* it is expanted to the return value of the parent program's execute */
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
    for(j = cur_idx; j < strlen((char*)buf); j++) {
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

    current_pid++; // stop at 6?
    process_control_block_t* pcb_new; // = (process_control_block_t*) 0x800000 - (0x2000 * current_pid); //(should be 0x800000 - PID * x)
    int8_t* pcb_start = (int8_t*) 0x800000 - (0x2000 * current_pid);


    pcb_new->pid = current_pid; // becomes 1 (on first time)
    pcb_new->parent_pid = current_parent_pid; // 0 - no parent yet

    pcb_new->tss_esp0 = 0x800000 - (0x2000 * (current_pid-1));
    
    /* initialzie file array */

    current_process = pcb_new;

    memcpy(pcb_start, pcb_new, 16); // 16 bytes need to change when we have file array

    return -1;

} 
