#include "syscalls.h"
#include "syscall_helper.h"
#include "descriptor.h"
#include "file.h"

/*
terminates a process & returns specified value to parent process
The system call handler itself is responsible for expanding the 8-bit argument \
 from BL into the 32-bit return value to the parent program’s execute system call

 Be careful not to return all 32 bits from EBX. 

 never return to caller

*/
int32_t halt (uint8_t status){
    return -1;
};


/*
attempts to load and execute a new program, then hands off the processor to the new program until it terminates

command is a space seperated series of words
1st word: file name of the program to be executed 
rest of the command:
    - should have leading spaces removed 
    - then passed to new program (on request) via the getargs system call

The execute call returns:
-1: if the command cannot be executed, ex:
    - if the program does not exist
    - the filename specified is not an executable
256: if the program dies by an exception
a value in the range 0 to 255: if the program executes a halt system call, in which case the value returned is that given by the program’s call to halt.
*/
int32_t execute (uint8_t* command){
    return execute_help(command);
};

/* int32_t read (int32_t fd, void* buf, int32_t nbytes);
 * Description: 
            int32_t fd      = file descriptor
            void* buf       = universal buffer
            int32_t nbytes  = how many bytes to read
 * Inputs: 
 * Return Value: 
 * Function:
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    /* if initial file pos is at or beyond end of file return 0*/
    if(files[fd].file_pos >= file_size){ return 0; }
    return (int32_t)files[fd].fotp.read;
};

/* int32_t write (int32_t fd, const void* buf, int32_t nbytes);
 * Description:
 * Inputs: 
 * Return Value:
 * Function:
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    return (int32_t)files[fd].fotp.write;
};

/* int32_t open (const uint8_t* filename);
 * Description: opens a file, allocates file descriptor if possible
 * Inputs: 
        const uint8_t* filename = file name obviously
 * Return Value: 0 on success, -1 if file doesn't exist OR no descriptors free
 * Function:
 */
int32_t open (const uint8_t* filename){
    /* open the file and check its valid*/
    if(file_open(filename) == -1){return -1;}

    /* allocate an unused file descriptor */
    if(cur_file.file_type == 0){
        return alloc_file(rtc_operations, cur_file.inode_num, 0);
    } else if (cur_file.file_type == 1){
        return alloc_file(dir_operations,cur_file.inode_num, 1);
    } else if (cur_file.file_type == 2){
        return alloc_file(file_operations, cur_file.inode_num, 2);
    }

    return 0;
};

/* int32_t close (int32_t fd);
 * Description:
 * Inputs: 
 * Return Value:
 * Function:
 */
int32_t close (int32_t fd){
    /* return -1 if they try to close stdin/out */
    if(fd == 0 || fd == 1){ return -1; }
    return free_file(fd);
};


int32_t getargs (uint8_t* buf, int32_t nbytes){
    return -1;
};


int32_t vidmap (uint8_t** screen_start){
    return -1;
};


int32_t set_handler (int32_t signum, void* handler){
    return -1;
};


int32_t sigreturn (void){
    return -1;
};
