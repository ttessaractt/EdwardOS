#include "types.h"
#include "file.h"
#include "kernel.h"
#include "keyboard.h"

dentry_t cur_file;
inode cur_file_det;
data_block data_buffer;

uint32_t file_open(const int8_t* fname){
    read_dentry_by_name(fname, &cur_file);
    int8_t* inode_addr = (int8_t*) boot_block_addr + 4096 + (cur_file.inode_num * 4096);
    memcpy(&cur_file_det.length, inode_addr, 4);
    return 0;
}

uint32_t file_close(){
    return 0;
}

uint32_t file_read(const int8_t* fname){
    file_open(fname);
    read_data(cur_file.inode_num, 0, data_buffer.data, cur_file_det.length);
    clear_screen();
    terminal_key_write(0, (char*)data_buffer.data, cur_file_det.length);
    printf("\nFile_name: %s", fname);
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

    // return value 0 - indicates end of file has been reached
    // return value -1 - improper inode number or data block number
    // return value num_bytes - returns how many bytes were written into the buffer

    /* only 63 files exist */

    int32_t num_inodes;
    memcpy(&num_inodes, (int8_t*)boot_block_addr+4, 4);

    if(inode < 0 || inode > (num_inodes - 1)) {
        return -1;;
    }

    int32_t num_data_blocks;
    memcpy(&num_data_blocks, (int8_t*)boot_block_addr+8, 4);

    int8_t* inode_addr = (int8_t*) boot_block_addr + 4096 + (inode * 4096);
    uint32_t data_block_num;
    inode_addr = inode_addr + 4;
    memcpy(&data_block_num, inode_addr, 4);
    if(data_block_num < 0 || data_block_num > (num_data_blocks - 1)) {
        return -1;
    }

    int8_t* data_start_addr = (int8_t*)boot_block_addr + 4096 + (num_inodes * 4096);
    int8_t* data_addr = data_start_addr + (data_block_num * 4096);

    data_addr = data_addr + offset;

    uint32_t bytes_written = 0;
    uint32_t cur_byte = offset;

    while(bytes_written != length) {
        data_buffer.data[bytes_written] = *data_addr;
        cur_byte = cur_byte + 1;
        bytes_written = bytes_written + 1;
        data_addr = data_addr + 1;
        if(cur_byte % 4096 == 0) {
            cur_byte = 0;
            inode_addr = inode_addr + 4;
            memcpy(&data_block_num, inode_addr, 4);
            if(data_block_num < 0 || data_block_num > (num_data_blocks - 1)) {
                return -1;
            }
            data_addr = data_start_addr + (data_block_num * 4096);
        }
    }
 
    return bytes_written;
}
