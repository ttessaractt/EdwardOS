#include "syscalls.h"
#include "syscall_helper.h"
#include "file.h"
#include "terminal.h"

/* halt
 * Description: calls halt_help, which performs the halt
 * Inputs: status - 8-bit arguemnt from BL
 * Return Value: from halt_help
 */
int32_t halt (uint8_t status){
    return halt_help(status);
};

/* execute
 * Description: calls execute_help, which performs the execute 
 * Inputs: command - space seperated sequence of words, 1st word = file name, rest = provided on request to getargs
 * Return Value: from execute_help
 */
int32_t execute (uint8_t* command){
    return execute_help(command);
};

/* read 
 * Description: reads data from keyboard, file, device (RTC), or directory
 * Inputs: int32_t fd - file descriptor
 *         buf - universal buffer
 *         nbytes - how many bytes to read
 * Return Value: number of bytes read
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    /* if initial file pos is at or beyond end of file return 0*/

    if(buf == NULL){
        return -1;
    }
    
    if (fd < 0 || fd > 7 || fd == 1){ // make sure to not run a write
        return -1;
    }

    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;
    //process_control_block_t* pcb_current = (process_control_block_t*) (MB_8 - (KB_8 * current_pid));
    //if(pcb_current->file_d_array[fd].file_pos >= file_size){ return 0; }
    if(pcb_current->file_d_array[fd].flags == 0){
        return -1;
    }

    return pcb_current->file_d_array[fd].fotp.read(fd, buf, nbytes);
    //return terminal_read(fd, buf, nbytes);
};

/* write
 * Description: writes data to the terminal or RTC
 * Inputs: fd - file descriptor
 *         buf - universal buffer
 *         nbytes - how many bytes to read
 * Return Value: number of bytes written - on success
 *               -1 - failure/read only file
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    if(buf == NULL){
        return -1;
    }
 
    if (fd < 0 || fd > 7 || fd == 0){ // make sure to not run a read
        return -1;
    }

    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;
    //process_control_block_t* pcb_current = (process_control_block_t*) (MB_8 - (KB_8 * current_pid));
    
    if(pcb_current->file_d_array[fd].flags == 0){
        return -1;
    }
    return pcb_current->file_d_array[fd].fotp.write(fd, buf, nbytes);
    //return terminal_write(fd, buf, nbytes);
};

/* open
 * Description: opens a file, allocates file descriptor if possible
 * Inputs: filename - file name 
 * Return Value: 0 - success
 *              -1 - if file doesn't exist OR no descriptors free
 */
int32_t open (const uint8_t* filename){
    /* open the file and check its valid*/
    if(filename == NULL){return -1;}
    if(filename == (uint8_t*)""){return -1;}
    if(strlen((int8_t*)filename) == 0){return -1;}
    
    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;
    //process_control_block_t* pcb_current = (process_control_block_t*) (MB_8 - (KB_8 * current_pid));
    dentry_t cur_d;
    if(read_dentry_by_name(filename, &cur_d) == -1){return -1;}
    

    int32_t location;
    /* allocate an unused file descriptor */
    if(cur_d.file_type == 0){            //if file is rtc
        location = alloc_file(rtc_operations, cur_d.inode_num, 0, pcb_current->file_d_array);
    } else if (cur_d.file_type == 1){    //if file is directory
        location = alloc_file(dir_operations,cur_d.inode_num, 1, pcb_current->file_d_array);
    } else if (cur_d.file_type == 2){    //if file is regular file
        location = alloc_file(file_operations, cur_d.inode_num, 2, pcb_current->file_d_array);
    }
    
    if (location != -1){
        pcb_current->file_d_array[location].fotp.open(filename);
    }

    return location;
};

/* int32_t close (int32_t fd);
 * Description: closes the specified file descriptor
 * Inputs: fd - file descriptor
 * Return Value: 0 - success
 *               -1 - invalid descriptor/failure
 */
int32_t close (int32_t fd){
    /* return -1 if they try to close stdin/out */
    if(fd == 0 || fd == 1 || fd > 8 || fd < 0){ return -1; }

    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;
    //process_control_block_t* pcb_current = (process_control_block_t*) (MB_8 - (KB_8 * current_pid));
    if(pcb_current->file_d_array[fd].flags == 0){
        return -1;
    }
    //pcb_current->file_d_array[fd].flags = 0;
    return free_file(&(pcb_current->file_d_array[fd]));
};

/* getargs
 * Description: not implemented yet
 * Inputs: 
 * Return Value: -1
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
    return getargs_helper(buf, nbytes);
};

/* vidmap
 * Description: not implemented yet
 * Inputs: 
 * Return Value: -1
 */
int32_t vidmap (uint8_t** screen_start){
    return vidmap_helper(screen_start);
};

/* set_handler
 * Description: not implemented yet
 * Inputs: 
 * Return Value: -1
 */
int32_t set_handler (int32_t signum, void* handler){
    return -1;
};

/* sigreturn
 * Description: not implemented yet
 * Inputs: 
 * Return Value: -1
 */
int32_t sigreturn (void){
    return -1;
};
