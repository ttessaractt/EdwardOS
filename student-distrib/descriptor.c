#include "types.h"
#include "file.h"
#include "kernel.h"
#include "keyboard.h"
#include "descriptor.h"
#include "terminal.h"

/*  filetypes:
 *  0: rtc
 *  1: directory
 *  2: regular file
 */

operations file_operations;
operations dir_operations;
operations rtc_operations;
operations stdin_operations;
operations stdout_operations;
file_info files[8]; 
int i;

void init_zero(){
    for(i = 0; i < 8; i++){
        files[i].flags = 0;
    }
}

int32_t alloc_file(operations operation, int32_t inode, int32_t file_type){
    for(i = 2; i < 8; i++){
        if(files[i].flags == 0){
            files[i].fotp = operation;

            //if filetype is not regular file, set inode to 0
            if(file_type != 2)
                files[i].inode = 0;
            else
                files[i].inode = inode;

            files[i].file_pos = 0;

            //0 is NOT USED, 1 is IN USE
            files[i].flags = 1;

            //return 0 on success
            return 0;
        }
    }
    //return -1 if array is full;
    return -1;
}

int32_t free_file(int32_t descriptor){
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

void init_std_op(){
    /* initialize stdin */
    files[0].fotp = stdin_operations;
    files[0].inode = 0;
    files[0].file_pos = 0;
    files[0].flags = 1;

    /*initialize stdout */
    files[1].fotp = stdout_operations;
    files[1].inode = 0;
    files[1].file_pos = 0;
    files[1].flags = 1;
}


