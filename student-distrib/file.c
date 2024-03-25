#include "types.h"
#include "file.h"
#include "kernel.h"
#include "keyboard.h"

dentry_t cur_file;
inode cur_file_det;
data_block data_buffer;
unsigned int num_dir_entries;
dentry_t cur_dir;
uint32_t dentry_index = -1;
uint32_t file_size;

/* uint32_t file_key_write(int32_t fd, char* buf, int32_t nbytes);
 * Description: writes a file to the terminal using the buffer
 * Inputs: int32_t fd, char* buf, int32_t nbytes
 * Return Value: 0 
 * Function: writes to terminal
 */
uint32_t file_key_write(uint32_t fd, char* buf, uint32_t nbytes){
    int i = 0;
    for (i = 0; i < nbytes; i++){
        if(buf[i] != '\0') {
            putc(buf[i]);
        }
    }

    return nbytes;


}

/* uint32_t file_open(const int8_t* fname);
 * Description: opens a file by 
                1) stores dentry entries into cur_file based on fname
                2) stores size into cur_file_det
 * Inputs: const int8_t* fname = file name
 * Return Value: 0 
 * Function: opens a file
 */
uint32_t file_open(const int8_t* fname){
    read_dentry_by_name(fname, &cur_file);
    int8_t* inode_addr = (int8_t*) boot_block_addr + BLOCK_LENGTH + 
        (cur_file.inode_num * BLOCK_LENGTH);
    memcpy(&cur_file_det.length, inode_addr, LENGTH_IN_BYTES_SIZE);
    return 0;
}

/* uint32_t file_close();
 * Description: closes file
 * Inputs: none
 * Return Value: 0
 * Function: nothing
 */
uint32_t file_close(){
    return 0;
}

/* uint32_t file_read(const int8_t* fname);
 * Description: reads a file and holds its contents
 * Inputs: const int8_t* fname = file name 
 * Return Value: 0
 * Function: reads a file
 */
uint32_t file_read(const int8_t* fname){
    if(cur_file.file_type == 2) {
		read_data(cur_file.inode_num, 0, data_buffer.data, cur_file_det.length);
        return 0;
	}
    return -1;
}

/* uint32_t file_write();
 * Description: writes a file
 * Inputs: none
 * Return Value: -1
 * Function: nothing
 */
uint32_t file_write(){
    return -1;
}

/* uint32_t directory_open();
 * Description: opens directory by incrementing to next dentry
 * Inputs: none
 * Return Value: 0
 * Function: increments dentry_index pointer 
 */
uint32_t directory_open(){
    memcpy(&num_dir_entries, (int8_t*)boot_block_addr, NUM_DIR_ENTRIES_SIZE);
    dentry_index = dentry_index + 1;
    if(dentry_index == num_dir_entries) {
        dentry_index = 0;
    }
    return 0;
}

/* uint32_t directory_close();
 * Description: 
 * Inputs: none
 * Return Value: 0
 * Function: nothing
 */
uint32_t directory_close(){
    return 0;
}

/* uint32_t directory_read();
 * Description: copies a dentry to a dentry buffer based on the dentry_index
 * Inputs: none
 * Return Value: 0
 * Function: reads a dentry 
 */
uint32_t directory_read(){
    read_dentry_by_index(dentry_index, &cur_dir);
    cur_dir.file_name[32] = '\0';
    int8_t* inode_addr = (int8_t*) boot_block_addr + BLOCK_LENGTH + 
        (cur_dir.inode_num * BLOCK_LENGTH);
    memcpy(&file_size, inode_addr, LENGTH_IN_BYTES_SIZE);
    return 0;
}

/* uint32_t directory_write();
 * Description: write to directory
 * Inputs: none
 * Return Value: -1
 * Function: nothing
 */
uint32_t directory_write(){
    return -1;
}

/* uint32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry);
 * Description: fills in dentry based on file name
 * Inputs:  const int8_t* fname = file name
            dentry_t* dentry    = dentry to fill out 
 * Return Value:
 * Function:
 */
uint32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry){

    int8_t* dir_start_addr = (int8_t*)boot_block_addr + SYS_STATISTICS_SIZE;
    int i;
    /* 63 directory entries */
    for(i = 0; i < MAX_NUM_OF_DIR_ENTRIES; i++) {
        /* file names match */
        if(strncmp(dir_start_addr, fname, FILE_NAME_SIZE) == 0) {
            /* copy fields to dentry structure */
            memcpy(&(dentry->file_name), dir_start_addr, FILE_NAME_SIZE);
            dir_start_addr = dir_start_addr + FILE_NAME_SIZE;

            memcpy(&(dentry->file_type), dir_start_addr, FILE_TYPE_SIZE);
            dir_start_addr = dir_start_addr + FILE_TYPE_SIZE;

            memcpy(&(dentry->inode_num), dir_start_addr, NUM_I_NODES_SIZE);

            /* successfully found file */
            return 0;
        }

        /* point to start of next directory entry */
        dir_start_addr = dir_start_addr + DENTRY_SIZE;
    }
    
    /* could not find file name, return -1 */
    return -1;
}

/* uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
 * Description: fills in dentry based on given index
 * Inputs:  uint32_t index = index of file to be read
            dentry_t* dentry = dentry to be filled out
 * Return Value: 0 on success, -1 if index is invalid
 * Function: fills in dentry
 */
uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

    /* only 63 dentries exist */
    if(index < 0 || index > MAX_NUM_OF_FILES) {
        return -1;
    }

    //get starting address of directories
    int8_t* dir_start_addr = (int8_t*)boot_block_addr + SYS_STATISTICS_SIZE;
    dir_start_addr = dir_start_addr + (DENTRY_SIZE * index);

    //fill in file name in dentry
    memcpy(&(dentry->file_name), dir_start_addr, FILE_NAME_SIZE);
    dir_start_addr = dir_start_addr + FILE_NAME_SIZE;

    //fill in file type in dentry
    memcpy(&(dentry->file_type), dir_start_addr, FILE_TYPE_SIZE);
    dir_start_addr = dir_start_addr + FILE_TYPE_SIZE;

    //fill in inode number in dentry
    memcpy(&(dentry->inode_num), dir_start_addr, I_NODE_NUM_SIZE);

    return 0;
    
}

/* uint32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length);
 * Description: puts file data into a read buffer 
 * Inputs:  uint32_t inode  = index node 
            uint32_t offset = offset of beginning of data to be read
            int8_t*  buf    = where we want to store the data
            uint32_t length = number of bytes we want to read to buf
 * Return Value:return value 0 - indicates end of file has been reached
                return value -1 - improper inode number or data block number
                return value num_bytes - returns how many bytes were written into the buffer
 * Function:
 */
uint32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length){
    //uint32_t* inode_start_addr = boot_block_addr + 4096 + (inode * 4096);
    // inode = inode number

    // return value 0 - indicates end of file has been reached
    // return value -1 - improper inode number or data block number
    // return value num_bytes - returns how many bytes were written into the buffer

    /* only 63 files exist */

    //put #inodes from bootblock into num_inodes
    int32_t num_inodes;
    memcpy(&num_inodes, (int8_t*)boot_block_addr + NUM_DIR_ENTRIES_SIZE, NUM_I_NODES_SIZE);

    //check if index node is valid
    if(inode < 0 || inode > (num_inodes - 1)) {
        return -1;;
    }

    //put #data blocks from bootblock into num_data_blocks
    int32_t num_data_blocks;
    memcpy(&num_data_blocks, (int8_t*)boot_block_addr + NUM_DIR_ENTRIES_SIZE + 
        NUM_I_NODES_SIZE, NUM_DATA_BLOCKS_SIZE);

    //put inode_addr at location of index node we want to access
    int8_t* inode_addr = (int8_t*) boot_block_addr + BLOCK_LENGTH + (inode * BLOCK_LENGTH);
    uint32_t data_block_num;

    //offset inode_addr to get index of 0th data block #
    inode_addr = inode_addr + LENGTH_IN_BYTES_SIZE;

    //get data block # from inode_addr and check if it is valid
    memcpy(&data_block_num, inode_addr, DATA_BLOCK_INDEX_SIZE);
    if(data_block_num < 0 || data_block_num > (num_data_blocks - 1)) {
        return -1;
    }

    //initialize data_start_addr to point data_addr to data we want to access
    int8_t* data_start_addr = (int8_t*)boot_block_addr + 
        BLOCK_LENGTH + (num_inodes * BLOCK_LENGTH);
    int8_t* data_addr = data_start_addr + (data_block_num * BLOCK_LENGTH);

    data_addr = data_addr + offset;

    uint32_t bytes_written = 0;
    uint32_t cur_byte = offset;

    //put data in data_buffer
    while(bytes_written != length) {
        data_buffer.data[bytes_written] = *data_addr;
        cur_byte = cur_byte + 1;
        bytes_written = bytes_written + 1;
        data_addr = data_addr + 1;
        if(cur_byte % BLOCK_LENGTH == 0) {
            //if we finish reading one block, go to the next block
            cur_byte = 0;
            inode_addr = inode_addr + DATA_BLOCK_INDEX_SIZE;
            memcpy(&data_block_num, inode_addr, DATA_BLOCK_INDEX_SIZE);
            //check if data_block_num is valid
            if(data_block_num < 0 || data_block_num > (num_data_blocks - 1)) {
                return -1;
            }
            data_addr = data_start_addr + (data_block_num * BLOCK_LENGTH);
        }
    }
 
    return bytes_written;
}