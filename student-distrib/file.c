#include "types.h"
#include "file.h"
#include "kernel.h"
#include "keyboard.h"

dentry_t cur_file;
data_block data_buffer;

uint32_t file_open(const int8_t* fname){
    read_dentry_by_name(fname, &cur_file);
    return 0;
}

uint32_t file_close(){
    return 0;
}

uint32_t file_read(const int8_t* fname){
    file_open(fname);
    read_data(cur_file.inode_num, 0, data_buffer.data, 8000);
    clear_screen();
    terminal_key_write(0, &(data_buffer.data), 187);
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

/* done */
uint32_t directory_read(){
    unsigned int i;
    unsigned int j;
    unsigned int num_dir_entries;
    memcpy(&num_dir_entries, (int8_t*)boot_block_addr, 4);

    clear_screen();

    for(i = 0; i < num_dir_entries; i++) {

        dentry_t cur_dir;
        read_dentry_by_index(i, &cur_dir);
        cur_dir.file_name[32] = '\0';

        int8_t* inode_addr = (int8_t*) boot_block_addr + 4096 + (cur_dir.inode_num * 4096);
        uint32_t file_size;
        memcpy(&file_size, inode_addr, 4);

        /* print values to terminal */
        printf("file name: ");
        printf("%s", cur_dir.file_name);
        printf(", file type: ");
        printf("%d", cur_dir.file_type);
        printf(", file size: ");
        printf("%d", file_size);
        printf("\n");
    }

    return 0;
}

/* done */
uint32_t directory_write(){
    return -1;
}

/* done, need to test */
uint32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry){

    int8_t* dir_start_addr = (int8_t*)boot_block_addr + 64;
    int i;
    /* 63 directory entries */
    for(i = 0; i < 63; i++) {
        /* file names match */
        if(strncmp(dir_start_addr, fname, 32) == 0) {
            /* copy fields to dentry structure */
            memcpy(&(dentry->file_name), dir_start_addr, 32);
            dir_start_addr = dir_start_addr + 32;

            memcpy(&(dentry->file_type), dir_start_addr, 4);
            dir_start_addr = dir_start_addr + 4;

            memcpy(&(dentry->inode_num), dir_start_addr, 4);

            /* successfully found file */
            return 0;
        }

        /* point to start of next directory entry */
        dir_start_addr = dir_start_addr + 64;
    }
    
    /* could not find file name, return -1 */
    return -1;
}

/* done, works */
uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

    /* only 63 dentries exist */
    if(index < 0 || index > 62) {
        return -1;
    }

    int8_t* dir_start_addr = (int8_t*)boot_block_addr + 64;
    dir_start_addr = dir_start_addr + (64 * index);

    memcpy(&(dentry->file_name), dir_start_addr, 32);
    dir_start_addr = dir_start_addr + 32;

    memcpy(&(dentry->file_type), dir_start_addr, 4);
    dir_start_addr = dir_start_addr + 4;

    memcpy(&(dentry->inode_num), dir_start_addr, 4);

    return 0;
    
}

uint32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length){
    //uint32_t* inode_start_addr = boot_block_addr + 4096 + (inode * 4096);
    // inode = inode number

    /* only 63 files exist */
    if(inode < 0 || inode > 62) {
        return -1;
    }
    int i;

    int32_t num_inodes;
    memcpy(&num_inodes, (int8_t*)boot_block_addr+4, 4);
    int8_t* inode_addr = (int8_t*) boot_block_addr + 4096 + (inode * 4096);
    uint32_t data_block_num;
    memcpy(&data_block_num, inode_addr + 4, 4);

    int8_t* data_start_addr = (int8_t*)boot_block_addr + 4096 + (num_inodes * 4096);
    int8_t* data_addr = data_start_addr + (data_block_num * 4096);

    // uint32_t data_length;
    // memcpy(&data_length, inode_addr, 4);

    // inode_addr = inode_addr + 4;

    //printf("Data %s", data_addr);
    for(i = 0; i < length; i++) {
        buf[i] = *data_addr;
        data_addr++;
    }

    //uint32_t data_start_block = offset / 4096;




    return 0;
}
