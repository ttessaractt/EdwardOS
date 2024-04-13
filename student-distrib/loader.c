#include "types.h"
#include "file.h"
#include "kernel.h"
#include "loader.h"

void program_loader(int8_t* file_name, int32_t task_number, dentry_t* dentry) {
    //printf("loader start HEYY\n");
    /* check for appropriate task number */
    if(task_number < 1 || task_number > 2) {
        return;
    }

    // int8_t* cur_addr;

    /* opens file and copies file data to data_buffer */
    
    //file_open((uint8_t*)file_name); // read dentry
    //read_dentry_by_name(file_name, DENTRYYYYY);
    int8_t* inode_addr = (int8_t*) boot_block_addr + BLOCK_LENGTH + 
        (dentry->inode_num * BLOCK_LENGTH);
    
    int32_t length = 0;

    memcpy(&length, inode_addr, 4);

    // char buffer[length];

    read_data(dentry->inode_num, 0, (int8_t*)PROGRAM_START, length); // read entire file data
    //printf("loader mid HEYY\n");
    /* choose where in physical memory to copy file data */
    // if(task_number == 1) {
    //     cur_addr = (int8_t*)0x0800000; // 8 mB ISSUE HERE
    // } else if(task_number == 2) {
    //     cur_addr = (int8_t*)0x0C00000; // 12 mB
    // }

    // commented out
    // cur_addr = (int8_t*)0x8048000;
    // //printf("loader mid2.0 HEYY\n");
    // int32_t i;

    // for(i = 0; i < length; i++) {
    //     *cur_addr = buffer[i];
    //     cur_addr = cur_addr + 1;
    // }

    //printf("loader end HEYY\n");
}

