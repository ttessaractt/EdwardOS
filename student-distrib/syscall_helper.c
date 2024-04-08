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

//global variables for functions
int32_t current_pid = 0;            // initial pid = 0
int32_t current_parent_pid = 0;     // initial parent pid = 0
process_control_block_t* current_process = NULL;    //initial PCB does not exist
operations file_operations;
operations dir_operations;
operations rtc_operations;
operations stdin_operations;
operations stdout_operations;
int location;
int ex_it = 0;
int pid_flag = 0;
int exit_halt = 0;
int32_t GOD = 0;

/* execute_help
 * Description: attempts to load and execute a new program, hands off processor to new program until it terminates
 * Inputs: command - space seperated sequence of words, 1st word = file name, rest = provided on request to getargs
 * Return Value: -1 - command cannot be executed
 *               256 - program dies by exception
 *               0-255 - program executes a halt system call, value given by halt
 */
int32_t execute_help(unsigned char* command){

    /*
    ex_it++;
    printf("Execute iteration: %d\n", ex_it);
    */

    /* check that command is not NULL */
    if (command == NULL){
        return -1;
    }

    /* resets to shell */
    if (pid_flag == 1){
        current_pid = 1;
        pid_flag = 0;
    }

    
    //create variables
    unsigned char file_name[MAX_FILE_NAME_LENGTH+1]; 
    unsigned char arguments[MAX_ARG_LENGTH]; 
    process_control_block_t* parent_pcb;

    // PARSE ARGS
    int32_t parse_check = parse_arguments(command, file_name, arguments); /* populates file_name */
    if (parse_check == -1){return -1;}
    // CHECK FILE VALIDITY
    int32_t entry_addr = check_file_validity((unsigned char*)file_name); /* entry_addr now stored in entry_addr */
    if(entry_addr == -1) {return -1;} /* not an executable file so return -1*/

    current_parent_pid = current_pid;       //initilizes parent pid to be 1st pid

    // CREATE PCB 
    initialize_pcb();

    tss.esp0 = current_process->tss_esp0;   //set esp0 for current process
    
    // SET UP PAGING
    allocate_tasks(current_process->pid);   // allocates memory for tasks 

    // LOAD FILE INTO MEMORY
    program_loader((char*)file_name, 1);

    // SAVE EBP
    register uint32_t saved_ebp asm("ebp"); // get ebp
    if (current_pid > 1){                   // save ebp in PCB for use later
        parent_pcb = (process_control_block_t*) MB_8 - (KB_8 * (current_pid-1));
        parent_pcb->ebp = saved_ebp;
    }

    // CONTEXT SWITCH AND IRET
    jump_to_user(entry_addr); 
 
    return 0;
}

/* halt_help
 * Description: terminates a process (if not base shell) & returns status to the parent process 
 * Inputs: status - 8-bit arguemnt from BL
 * Return Value: 1
 */
int32_t halt_help(unsigned char status){
    // create variables
    process_control_block_t* pcb_parent;
    int b;

    // get the esp0 of the parent 
    if (current_process->parent_pid == 0){
        //pcb_parent = (process_control_block_t*) MB_8 - (KB_8 * (current_pid));
        exit_halt = 1;
    }
    else{
        pcb_parent = (process_control_block_t*) MB_8 - (KB_8 * (current_process->parent_pid));
        exit_halt = 0;
    } 

    
    if (exit_halt){

        pid_flag = 0;
        exit_halt = 0;
        current_pid = 0;
        execute_help((uint8_t*)"shell");
    }


    // set the new esp0    
    tss.esp0 = pcb_parent->tss_esp0;
    tss.ss0 = KERNEL_DS;                    

    // restore parent paging
    if (current_process->parent_pid != 0){
        allocate_tasks(pcb_parent->parent_pid + 1);
    }

    // close relevent fd's
    for(b = FIRST_FILE_OFFSET; b < FD_ARRAY_LEN; b++) {
        current_process->file_d_array[b].flags = 0; // set all files to unused;
    }

    // current process becomes parent
    current_process = pcb_parent;

    // expand 8-bit input to 32-bits
    uint32_t stat = (uint32_t)status;

    pid_flag = 1;

    if (GOD){
        GOD = 0;
        stat = (uint32_t)EXCEPTION;
    }

    //call halt assembly code 
    halt_asm(pcb_parent->ebp, stat);

    return 1;
}

/* parse_arguments
 * Description: get the file name to be executed & strip the rest of command of leading spaces
 * Inputs: buf - buffer with the command to be parsed
 *         file_name - buffer to place the file name into
 *         arguemnts - buffer to place the rest of command into
 * Return Value: 1 - success
 */
int32_t parse_arguments(unsigned char* buf, unsigned char* file_name, unsigned char* arguments){
    /* check for NULL buffer */
    if (buf == NULL){
        return -1;      /* return -1 for fail */
    }

    /* filter out spaces from first file name */
    int cur_idx = 0;
    int old_cur_idx = 0;
    while(buf[cur_idx] == SPACE_CHAR) {
        cur_idx++;
    }

    /* get first file name */
    int i = 0;
    old_cur_idx = cur_idx;
    while(buf[cur_idx] != SPACE_CHAR) {
        if(cur_idx > (strlen((char*)buf))) {
            file_name[i] = '\0';
            return 1;
        }

        file_name[i] = buf[cur_idx];
        i++;
        cur_idx++;
        if (i > FILE_NAME_SIZE){
            return -1;
        }
    }
    file_name[i] = '\0';

    /* filter out spaces between first file name and next */
    while(buf[cur_idx] == SPACE_CHAR) {
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

    return 1; //success
}

/* initialize_pcb
 * Description: initilizes the PCB for execute
 * Inputs: none
 * Return Value: 0 - success
 */
int32_t initialize_pcb(){
    // create variables
    int i;
    current_pid++; 

    // make new PCB
    process_control_block_t* pcb_new = (process_control_block_t*) MB_8 - (KB_8 * current_pid); //(should be MB_8 - PID * x)
    pcb_new->pid = current_pid;                 // becomes 1 (on first time)
    pcb_new->parent_pid = current_parent_pid;   // 0 - no parent yet
    pcb_new->tss_esp0 = MB_8 - (KB_8 * (current_pid-1));

    /* initialzie file array */
    file_info files[FD_ARRAY_LEN]; 
    init_file_operations();
    init_std_op(files);
    for(i = 0; i < FD_ARRAY_LEN; i++){
        pcb_new->file_d_array[i] = files[i];
    }
    current_process = pcb_new;
    return 0;
} 

/* init_zero
 * Description: clear file descriptor array
 * Inputs: files - file descriptor array
 * Return Value: none
 */
void init_zero(file_info* files){
     for(location = 0; location < FD_ARRAY_LEN; location++){
         files[location].flags = 0;
     }
 }

/* alloc_file
 * Description: finds next open spot in file descriptor array & adds entry with necessary information
 * Inputs: operation - operation of file
 *         indoe - inode of file
 *         file_type - type of file
 *         files - file descriptor array
 * Return Value: location - position in file descriptor array
 *               -1 - array full/error
 */
int32_t alloc_file(operations operation, int32_t inode, int32_t file_type, file_info* files){
    for(location = FIRST_FILE_OFFSET; location < FD_ARRAY_LEN; location++){
        if(files[location].flags == 0){
            files[location].fotp = operation;

            //if filetype is not regular file, set inode to 0
            if(file_type != NORMAL_FILE_NUM)
                files[location].inode = 0;
            else
                files[location].inode = inode;

            files[location].file_pos = 0;

            //0 is NOT USED, 1 is IN USE
            files[location].flags = 1;

            //return location on success
            return location;
        }
    }
    //return -1 if array is full;
    return -1;
}

/* free_file
 * Description: frees entry in file descriptor array
 * Inputs: descriptor - location in file descriptor array
 *         files - file descriptor array
 * Return Value: 0 - success
 *               -1 - trying to close file not in use/error
 */
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

/* init_file_operations
 * Description: initilizes file descriptor array for file, dir, rtc, and parts of stdin and stdout
 * Inputs: none
 * Return Value: none
 */
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

/* init_std_op
 * Description: finished initialization of stdin and stdout for the file descriptor array
 * Inputs: files - file descriptor array
 * Return Value: none
 */
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

