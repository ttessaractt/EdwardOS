#include "types.h"
#include "file.h"
#include "kernel.h"
#include "keyboard.h"
#include "descriptor.h"

/*  filetypes:
 *  0: rtc
 *  1: directory
 *  2: regular file
 */

file_info files[8]; 
int i;

void init_zero(){
    for(i = 0; i < 8; i++){
        files[i].flags = 0;
    }
}

int32_t alloc_file(operations* operation, int32_t inode, int32_t file_type){
    for(i = 2; i < 8; i++){
        if(files[i].flags == 0){
            files[i].fotp = &operation;

            //if filetype is not regular file, set inode to 0
            if(file_type != 2)
                files[i].inode = 0;
            else
                files[i].inode = inode;

            files[i].file_pos = i;
            files[i].flags = 1;

            //return 0 on success
            return 0;
        }
    }
    //return -1 if array is full;
    return -1;
}

int32_t free_file(int32_t descriptor){
    if(files[descriptor].flags != 0){
        return -1;
    } else if(files[descriptor].flags == 1){
        files[descriptor].fotp = 0;
        files[descriptor].inode = 0;
        files[descriptor].file_pos = 0;
        files[descriptor].flags = 0;
        return 0;
    }
    return -1;
}


