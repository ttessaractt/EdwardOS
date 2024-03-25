#include "types.h"
#include "file.h"
#include "kernel.h"

uint32_t num_of_dir_entries;


uint32_t file_open(const uint8_t* fnamme, dentry_t* dentry){
    //num_of_dir_entries = *(boot_block_addr);
    read_dentry_by_name(fnamme, dentry);
    return 0;
}

uint32_t file_close(){
    return 0;
}

uint32_t file_read(const uint8_t* fname){

    return 0;
}



/* done */
uint32_t file_write(){
    return -1;
}

uint32_t directory_open(){
    return 0;
}

/* done */
uint32_t directory_close(){
    return 0;
}


uint32_t directory_read(){
    unsigned long dir_entries = boot_block_addr + (64 * 8);
    unsigned long 
    int i, j;


    for(i = 0; i < 63; ++i){
        for(j = 0; j < 32; ++j){
            //dir_en 
        }
    }


    return 0;
}

/* done */
uint32_t directory_write(){
    return -1;
}

/*
When successful, the first two calls fill in the dentry t block passed as their second argument with the file name, file
type, and inode number for the file, then return 0. 
*/
uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    //if file name is invalid
    if(fname == NULL){ return -1; }

    //check fname == file_name in dentry
    int counter = 0;
    while(*(fname + counter) != NULL){
        if(dentry->file_name[counter] != *(fname + counter)){ return -1; }
        ++counter;
    }

    return 0;
}

uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    dentry->inode_num = index;
    return 0;
    
}

uint32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

    // inode = inode number
    // 
    return 0;
}
