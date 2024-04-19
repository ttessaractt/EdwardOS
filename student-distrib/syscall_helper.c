/* syscall_helper.c - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */


#include "lib.h"
#include "file.h"
#include "syscall_helper.h"
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
int program_counter = 0;
int initial_shell_flag = 0;     //flag for if shell is base shell
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
    
    //create variables
    unsigned char file_name[MAX_FILE_NAME_LENGTH+1]; 
    unsigned char arguments[MAX_ARG_LENGTH]; 
    process_control_block_t* parent_pcb;
    dentry_t new_dentry;

    // PARSE ARGS
    int32_t parse_check = parse_arguments(command, file_name, arguments); /* populates file_name */
    if (parse_check == -1){return -1;}
    // CHECK FILE VALIDITY
    int32_t entry_addr = check_file_validity((unsigned char*)file_name, &new_dentry); /* entry_addr now stored in entry_addr */
    if(entry_addr == -1) {return -1;} /* not an executable file so return -1*/

    current_parent_pid = current_pid;       //initilizes parent pid to be 1st pid

    //check if shell being created in base shell
    if(!(strncmp((int8_t*)file_name, "shell\0", 6)) && current_pid == 0){
        initial_shell_flag = 1;
    }

    //check for maximum number of programs
    if(current_pid == 6){
            printf("Maximum number of programs\n");
            return 0;
    }        

    // CREATE PCB 
    initialize_pcb();

    // set PCB entires for getargs
    //current_process->arguments[0] = arguments; //set arguments in PCB
    current_process->arg_length = strlen((char*)arguments);
    int j;
    for (j = 0; j < current_process->arg_length; j++){
        current_process->arguments[j] = arguments[j];
    }

    current_process->cur_file_dentry = new_dentry;

    tss.esp0 = current_process->tss_esp0;   //set esp0 for current process
    
    // SET UP PAGING
    allocate_tasks(current_process->pid);   // allocates memory for tasks 

    // LOAD FILE INTO MEMORY
    program_loader((char*)file_name, 1, &(current_process->cur_file_dentry)) ;

    // SAVE EBP
    register uint32_t saved_ebp asm("ebp"); // get ebp
    if (current_pid > 1){                   // save ebp in PCB for use later
        int32_t parent_pcb_addr2 = calculate_pcb_addr((current_process->pid) - 1);
        parent_pcb = (process_control_block_t*) parent_pcb_addr2;
        //parent_pcb = (process_control_block_t*) MB_8 - (KB_8 * (current_pid-1));
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
    if (current_process->base_shell == 1){      //check if in base shell
        //if in base shell, restart shell
        --current_pid;
        current_pid = 0;
        printf("Restarting Shell...\n");
        return execute_help((uint8_t*)"shell");
    }
    else{
        int32_t pcb_parent_addr = calculate_pcb_addr(current_process->parent_pid);
        pcb_parent = (process_control_block_t*) pcb_parent_addr;
        //pcb_parent = (process_control_block_t*) (MB_8 - (KB_8 * (current_process->parent_pid)));
    } 

    // set the new esp0   
    // tss.esp0 = (MB_8 - (KB_8 * ((current_process->parent_pid)-1)));
    tss.esp0 = pcb_parent->tss_esp0;
    tss.ss0 = KERNEL_DS;                    

    // restore parent paging
    if (current_process->parent_pid != 0){
        allocate_tasks(current_process->parent_pid); // was pcb_parent->parent_pid + 1 before
    }

    // close relevent fd's
    for(b = FIRST_FILE_OFFSET; b < FD_ARRAY_LEN; b++) {
        current_process->file_d_array[b].flags = 0; // set all files to unused;
    }

    // current process becomes parent
    current_process = pcb_parent; //decrement current_pid
    --current_pid; 
    // expand 8-bit input to 32-bits
    uint32_t stat = (uint32_t)status;

    if (GOD){
        GOD = 0;
        stat = (uint32_t)EXCEPTION;
    }

    //call halt assembly code 
    halt_asm(pcb_parent->ebp, stat);

    return 1;
}

/* vidmap_helper
 * Description: Maps program to point to video memory page in userspace which maps to vid mem
 * Inputs: screen_start - double pointer to start of sreen
 * Return Value: -1 (out of bounds)
 *               200 MB (successfully mapped)
 */
int32_t vidmap_helper(uint8_t** screen_start) {
    /* OFFSET_VID_MEM_START = 0xB8000 */
    /* check if out of bounds */
    if((int32_t)screen_start < (int32_t)OFFSET_128MB || (int32_t)screen_start > (int32_t)(OFFSET_128MB + OFFSET_4MB)) {
        return -1;
    }

    /* adds a video memory page at 200MB for user space that maps to B8000-B900*/
    add_vid_mem_page();

    /* make screen start pointer point to our user space page*/
    *screen_start = (uint8_t*) OFFSET_200;
    return (int32_t)OFFSET_200;
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
    int j;
    int term_num;
    current_pid++; 

    // make new PCB

    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_new = (process_control_block_t*) pcb_addr;

    // process_control_block_t* pcb_new = (process_control_block_t*) (MB_8 - (KB_8 * current_pid)); //(should be MB_8 - PID * x)
    pcb_new->pid = current_pid;                 // becomes 1 (on first time) # page fault here?
    pcb_new->parent_pid = current_parent_pid;   // 0 - no parent yet // current pid = 3??
    pcb_new->tss_esp0 = (MB_8 - (KB_8 * (current_pid-1))); // first 8MB, then 8MB - 8KB

    /* find the active terminal index */
    term_num = get_active_term();

    pcb_new->terminal_id = term_num + 1;

    //set pcb entry if pcb being created is for the base shell
    if (initial_shell_flag == 1){
        pcb_new->base_shell = 1;
        initial_shell_flag = 0;
    }
    else{
        pcb_new->base_shell = 0;
        initial_shell_flag = 0;
    }

    //initilize getargs arguments to 0
    pcb_new->arg_length = 0;
    for (j = 0; j < 1024; j++){
        pcb_new->arguments[j] = NULL;
    }

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
int32_t free_file(file_info* files){
    if(files->flags == 0){ 
        return -1; // return -1 if you trying to close a file that is not in use
    } else if(files->flags == 1){
        //files[descriptor].fotp = (*operations)0;
        files->inode = 0;
        files->file_pos = 0;
        files->flags = 0;
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
    files[0].file_pos = 0; // changed to 0
    files[0].flags = 1;

    /*initialize stdout */
    files[1].fotp = stdout_operations;
    files[1].inode = 0;
    files[1].file_pos = 0;
    files[1].flags = 1;
}

/* getargs_helper
 * Description: helper function for getargs that checks for validity and copies args to buffer
 * Inputs: buf - buffer to copy args into
 *         nbytes - size of buffer
 * Return Value: none
 */
int32_t getargs_helper(uint8_t* buf, int32_t nbytes){
    int i;
    //check validity
    if(current_process->arg_length == (uint32_t)FOUR && current_process->arguments[0] == (uint32_t)MAX) {return -1;}
    if(current_process->arguments[0] == NULL){return -1;}
    if(current_process->arg_length <= 0){return -1;}
    if (current_process->arg_length > MAX_FILE_NAME_LENGTH){return -1;}
    //copy arguments to buffer
    for (i = 0; i < current_process->arg_length; i++){
        buf[i] = (current_process->arguments)[i];
    }
    //add null terminator to buffer
    buf[i] = '\0';
    return 0;
};

