/* syscall_helper.c - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "syscall_helper.h"

#include "lib.h"
#include "file.h"
#include "paging.h"
#include "loader.h"
#include "x86_desc.h"
#include "iret_helper.h"
#include "terminal.h"
#include "rtc.h"
#include "halt.h"
#include "types.h"

int32_t current_pid = 0; // initial pid = 0
int32_t current_parent_pid = 0;
process_control_block_t* current_process = NULL;
operations file_operations;
operations dir_operations;
operations rtc_operations;
operations stdin_operations;
operations stdout_operations;
int location;
int ex_it = 0;

int32_t execute_help(unsigned char* command){
    ex_it++;
    printf("Execute iteration: %d\n", ex_it);
    //cli();

    if (command == NULL){
        return -1;
    }

    unsigned char file_name[32+1]; // 32+1?
    unsigned char arguments[128]; // check lengths?

    // PARSE ARGS
    /* populates file_name */
    printf("before parse\n");
    int32_t parse_check = parse_arguments(command, file_name, arguments);
    if (parse_check == -1){
        return -1;
    }
    printf("%s\n", file_name);
    // CHECK FILE VALIDITY
    int32_t entry_addr = check_file_validity((unsigned char*)file_name);
    /* not an executable file */
    if(entry_addr == -1) {
        return -1;
    }

    /* entry_addr now stored in entry_addr */
    current_parent_pid = current_pid;

    // CREATE PCB 
    initialize_pcb(file_name);

    tss.esp0 = current_process->tss_esp0;
    // SET UP PAGING
    // allocates memory for tasks 
    allocate_tasks(current_process->pid);

    //flush_tlb();
    // LOAD FILE INTO MEMORY
    program_loader((char*)file_name, 1);
    

    process_control_block_t* parent_pcb;

    //printf("before jump\n");
    //printf("%x\n", entry_addr);
    register uint32_t saved_ebp asm("ebp");

    //uint32_t saved_ebp = 0x7fff80;
    if (current_pid > 1){
        parent_pcb = (process_control_block_t*) 0x800000 - (0x2000 * (current_pid-1));
        parent_pcb->ebp = saved_ebp;
    }
    // CONTEXT SWITCH AND IRET
    jump_to_user(entry_addr); // 
    //printf("dont come back");

    return 0;

}

//status is return value from main (if things worked)
int32_t halt_help(unsigned char status){
    printf("start halt\n");
    /* not done! */
    //restore parent stack stuff
    /* get the esp0 of the parent */
    process_control_block_t* pcb_parent;
    if (current_process->parent_pid == 0){
        pcb_parent = (process_control_block_t*) 0x800000 - (0x2000 * (current_pid));
    }
    else{
        pcb_parent = (process_control_block_t*) 0x800000 - (0x2000 * (current_process->parent_pid));
    } 
    
        
    tss.esp0 = pcb_parent->tss_esp0;
    //0x800000;
    //printf("%d\n", current_process->parent_pid);
    //pcb_parent->tss_esp0;        //change tss
    tss.ss0 = KERNEL_DS;                    //shouldnt be changed but make sure it remains the same

    //current_pid = 0;
    //restore parent paging
    if (current_process->parent_pid != 0){
        allocate_tasks(pcb_parent->parent_pid+1);
    }

    //close relevent fd's
    int b;
    for(b = 2; b < 8; b++) {
        current_process->file_d_array[b].flags = 0; // set all files to unused;
    }

    current_process = pcb_parent;
    //make sure return val is in eax

    //jump to execute return

    /* the 8 bit input is BL (register) which should then be expanded */
    /* it is expanded to the return value of the parent program's execute */
    uint32_t stat = (uint32_t)status;

    halt_asm(pcb_parent->ebp, stat);

    return 1;

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
    printf("before second while\n");
    /* at first file name */
    int i = 0;
    while(buf[cur_idx] != 0x20) {
        if(cur_idx >= strlen((char*)buf)) {
            printf("leaving little bro\n");
            printf("%s\n", file_name);
            file_name[cur_idx] = '\0';
            return 1;
        }
        file_name[i] = buf[cur_idx];
        i++;
        cur_idx++;
        if(i > 32){
            printf("i over 32?\n");
            return -1;
        }
    }
    file_name[i] = '\0';
    printf("%s", file_name);
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
    //printf("\n");
    //printf("%s HIIII", file_name);
    //printf("\n");
    //printf("%s", arguments);

    return 1; //success

}

int32_t initialize_pcb(unsigned char* file_name){
    //printf("PBC StART HEYY\n");
    int i;
    current_pid++; // stop at 6?
    process_control_block_t* pcb_new = (process_control_block_t*) 0x800000 - (0x2000 * current_pid); //(should be 0x800000 - PID * x)
    //int8_t* pcb_start = (int8_t*) 0x800000 - (0x2000 * current_pid);


    pcb_new->pid = current_pid; // becomes 1 (on first time)
    pcb_new->parent_pid = current_parent_pid; // 0 - no parent yet

    pcb_new->tss_esp0 = 0x800000 - (0x2000 * (current_pid-1));
    //pcb_new->ebp = 0x800000 - (0x2000 * (current_pid-1));
    /* initialzie file array */

    file_info files[8]; 

    init_file_operations();
    init_std_op(files);
    
    for(i = 0; i < 8; i++){
        pcb_new->file_d_array[i] = files[i];
    }

    current_process = pcb_new;
    //printf("PBC mcpy HEYY\n");
    //memcpy(pcb_start, pcb_new, 16); // 16 bytes need to change when we have file array
    //printf("PBC end HEYY\n");
    return 0;

} 

void init_zero(file_info* files){
     for(location = 0; location < 8; location++){
         files[location].flags = 0;
     }
 }

int32_t alloc_file(operations operation, int32_t inode, int32_t file_type, file_info* files){
    for(location = 2; location < 8; location++){
        if(files[location].flags == 0){
            files[location].fotp = operation;

            //if filetype is not regular file, set inode to 0
            if(file_type != 2)
                files[location].inode = 0;
            else
                files[location].inode = inode;

            files[location].file_pos = 0;

            //0 is NOT USED, 1 is IN USE
            files[location].flags = 1;

            //return 0 on success
            return location;
        }
    }
    //return -1 if array is full;
    return -1;
}

int32_t free_file(int32_t descriptor, file_info* files){
    if(files[descriptor].flags == 0){ 
        return -1; // return -1 if you trying to close a file that is not in use
    } else if(files[descriptor].flags == 1){
        //files[descriptor].fotp = (*operations)0;
        files[descriptor].inode = 0;
        files[descriptor].file_pos = 0;
        files[descriptor].flags = 0;
        return 0;
    }
    return -1;
}

void init_file_operations(){
    /* file operation */
    file_operations.open = file_open;
    file_operations.close = file_close;
    file_operations.read = file_read;
    file_operations.write = file_write;

    /* dir operations */
    dir_operations.open = directory_open;
    dir_operations.close = directory_close;
    dir_operations.read = directory_read;
    dir_operations.write = directory_write;

    /* rtc operations */
    rtc_operations.open = RTC_open;
    rtc_operations.close = RTC_close;  
    rtc_operations.read = RTC_read;
    rtc_operations.write = RTC_write;

    /* stdin operations*/
    stdin_operations.read = terminal_read;

    /* stdout operations */
    stdout_operations.write = terminal_write;
}

void init_std_op(file_info* files){
    /* initialize stdin */
    init_zero(files);

    files[0].fotp = stdin_operations;
    files[0].inode = 0;
    files[0].file_pos = -1;
    files[0].flags = 1;

    /*initialize stdout */
    files[1].fotp = stdout_operations;
    files[1].inode = 0;
    files[1].file_pos = -1;
    files[1].flags = 1;
}
