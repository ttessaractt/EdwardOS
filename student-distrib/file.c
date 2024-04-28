#include "types.h"
#include "file.h"
#include "kernel.h"
#include "keyboard.h"
//#include "descriptor.h" 
#include "terminal.h"
//#include "syscall_helper.h"

dentry_t cur_file;
inode cur_file_det;
data_block data_buffer;
unsigned int num_dir_entries;
dentry_t cur_dir;
int32_t dentry_index = -1;
uint32_t file_size;


/* uint32_t file_key_write(int32_t fd, char* buf, int32_t nbytes);
 * Description: writes a file to the terminal using the buffer
 * Inputs: int32_t fd, char* buf, int32_t nbytes
 * Return Value: 0 
 * Function: writes to terminal
 */
int32_t file_key_write(uint32_t fd, char* buf, uint32_t nbytes){
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
int32_t file_open(const uint8_t* fname){
    /*  filetypes:
    *  0: rtc
    *  1: directory
    *  2: regular file
    */
    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;
    

    if(read_dentry_by_name(fname, &(pcb_current->cur_file_dentry)) == -1){return -1;}
    // int8_t* inode_addr = (int8_t*) boot_block_addr + BLOCK_LENGTH + 
    //     (pcb_current->cur_file_dentry.inode_num * BLOCK_LENGTH);

    // if(strncmp("hello", fname, 5) == 0) {
    //    cur_file_det.length = 5349;
    // } else {
    //memcpy(&cur_file_det.length, inode_addr, LENGTH_IN_BYTES_SIZE);
    //}

    

    return 0;
}

/* uint32_t file_close();
 * Description: closes file
 * Inputs: none
 * Return Value: 0
 * Function: nothing
 */
int32_t file_close(int32_t fd){
    return 0;
}

/* uint32_t file_read(const int8_t* fname);
 * Description: reads a file and holds its contents
 * Inputs: const int8_t* fname = file name 
 * Return Value: 0
 * Function: reads a file
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
    //printf("%d", current_pid);
    int32_t pos = 0;

    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;
    //process_control_block_t* pcb_current = (process_control_block_t*) (MB_8 - (KB_8 * current_pid));

    if(pcb_current->cur_file_dentry.file_type == 2) {
		pos = read_data(pcb_current->file_d_array[fd].inode, pcb_current->file_d_array[fd].file_pos, buf, nbytes);
        pcb_current->file_d_array[fd].file_pos += pos;
        return pos; // BIG PROBLEM
	}
    return -1;
}

/* uint32_t file_write();
 * Description: writes a file
 * Inputs: none
 * Return Value: -1
 * Function: nothing
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* uint32_t directory_open();
 * Description: opens directory by incrementing to next dentry
 * Inputs: none
 * Return Value: 0
 * Function: increments dentry_index pointer 
 */
int32_t directory_open(const uint8_t* filename){
    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;

    if(read_dentry_by_name(filename, &(pcb_current->cur_file_dentry)) == -1){return -1;}
    dentry_index = 0; // put in pcb ?
    return 0; // read dentry by name, index = 0
}

/* uint32_t directory_close();
 * Description: 
 * Inputs: none
 * Return Value: 0
 * Function: nothing
 */
int32_t directory_close(int32_t fd){
    return 0;
}

/* uint32_t directory_read();
 * Description: copies a dentry to a dentry buffer based on the dentry_index
 * Inputs: none
 * Return Value: 0
 * Function: reads a dentry 
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes){

    int32_t pcb_addr = calculate_pcb_addr(current_pid);
    process_control_block_t* pcb_current = (process_control_block_t*) pcb_addr;

    read_dentry_by_index(dentry_index, &(pcb_current->cur_file_dentry));
    pcb_current->cur_file_dentry.file_name[32] = '\0';
    strncpy(buf, pcb_current->cur_file_dentry.file_name, 32);

    dentry_index = dentry_index + 1;
    if(dentry_index == num_dir_entries) {
         dentry_index = 0;
    }

    if (strlen(pcb_current->cur_file_dentry.file_name) < 32){
        return strlen(pcb_current->cur_file_dentry.file_name);
    }
    else{
        return 32;
    }

}

/* uint32_t directory_write();
 * Description: write to directory
 * Inputs: none
 * Return Value: -1
 * Function: nothing
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
 * Description: fills in dentry based on file name
 * Inputs:  const int8_t* fname = file name
            dentry_t* dentry    = dentry to fill out 
 * Return Value:
 * Function:
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){

    int8_t* dir_start_addr = (int8_t*)boot_block_addr + SYS_STATISTICS_SIZE;
    int i;
    /* 63 directory entries */
    for(i = 0; i < MAX_NUM_OF_DIR_ENTRIES; i++) {
        /* file names match */
        if(strncmp(dir_start_addr, (int8_t*)fname, FILE_NAME_SIZE) == 0) {
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
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){

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
int32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length){
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

    //check if at end of file
    int8_t* inode_addr_cur = (int8_t*) boot_block_addr + BLOCK_LENGTH + 
    (inode * BLOCK_LENGTH);

    int32_t file_length;
    memcpy(&file_length, inode_addr_cur, 4);

    if(offset == file_length) {
        return 0;
    }

    //put #data blocks from bootblock into num_data_blocks
    int32_t num_data_blocks;
    memcpy(&num_data_blocks, (int8_t*)boot_block_addr + NUM_DIR_ENTRIES_SIZE + 
        NUM_I_NODES_SIZE, NUM_DATA_BLOCKS_SIZE);

    //put inode_addr at location of index node we want to access
    int8_t* inode_addr = (int8_t*) boot_block_addr + BLOCK_LENGTH + (inode * BLOCK_LENGTH);
    uint32_t data_block_num;

    //offset inode_addr to get index of 0th data block #
    inode_addr = inode_addr + LENGTH_IN_BYTES_SIZE + ((int)(offset / 4096) * DATA_BLOCK_INDEX_SIZE);

    //get data block # from inode_addr and check if it is valid
    memcpy(&data_block_num, inode_addr, DATA_BLOCK_INDEX_SIZE);
    if(data_block_num < 0 || data_block_num > (num_data_blocks - 1)) {
        return -1;
    }

    //initialize data_start_addr to point data_addr to data we want to access
    int8_t* data_start_addr = (int8_t*)boot_block_addr + 
        BLOCK_LENGTH + (num_inodes * BLOCK_LENGTH);
    int8_t* data_addr = data_start_addr + (data_block_num * BLOCK_LENGTH);

    data_addr = data_addr + (offset % 4096);

    uint32_t bytes_written = 0;
    uint32_t cur_byte = offset;

    //put data in data_buffer
    
    /* no longer clobbering first PCB here */
    while(bytes_written != length) {

        if(cur_byte >= 4096) {
            //if we finish reading one block, go to the next block      
            cur_byte = offset % 4096;                         
            if(bytes_written > 0){
                inode_addr = inode_addr + DATA_BLOCK_INDEX_SIZE;                
                memcpy(&data_block_num, inode_addr, DATA_BLOCK_INDEX_SIZE);     
                //check if data_block_num is valid
                if(data_block_num < 0 || data_block_num > (num_data_blocks - 1)) {
                    return -1;
                }
                data_addr = data_start_addr + (data_block_num * BLOCK_LENGTH);
            }                 
            
        }
        buf[bytes_written] = *data_addr;
        cur_byte = cur_byte + 1;
        bytes_written = bytes_written + 1;
        data_addr = data_addr + 1;
        if(bytes_written+offset >= file_length){
            return bytes_written;
        }
    }

    return bytes_written;
}

/* returns -1 if file is invalid, else returns entry point address */
int32_t check_file_validity(uint8_t* fname, dentry_t* dentry) {
    /* want to make sure file is an executable file*/
    /* header inside first 40 bytes */
    int i, j, k;
    /* put file data into data_buffer.data */
    
    j = read_dentry_by_name(fname, dentry);
    
    char buffer[32];

    if (j == -1){
        return -1;
    }
    k = read_data(dentry->inode_num, 0, buffer, 32); // offset 0 because no file pos yet
    if (k == -1){
        return -1;
    }


    /* first 4 bytes represent a magic number that identifies file
       as being executable */
    /* if magic number is not present, execute should fail */
    int8_t* data_ptr = buffer;
    for(i = 0; i < 4; i++) {
        if(i == 0) {
            if(*data_ptr != 0x7F) {
                return -1;
            }
        } else if(i == 1) {
            if(*data_ptr != 0x45) {
                return -1;
            }
        } else if(i == 2) {
            if(*data_ptr != 0x4c) {
                return -1;
            }
        } else if(i == 3) {
            if(*data_ptr != 0x46) {
                return -1;
            }
        }

        data_ptr++;
    }

    /* at this point, we know the file is a valid executable */

    /* bytes 24-27 contain the entry point into the program */
    /* need to save this */

    /* double check pointer stuff is accurate */
    uint32_t* data_ptr2 = (uint32_t*)(buffer + 24);
    uint32_t entry_point_addr = *data_ptr2;

    return entry_point_addr;

}

/* int32_t calculate_pcb_addr(int32_t cur_pid);
 * Description: calculate the pcb addr given current pid index
 * Inputs:  int32_t cur_pid = current pid index
 * Return Value: the address of the current pid 
 */
int32_t calculate_pcb_addr(int32_t cur_pid) {
    return (int32_t)(MB_8 - (KB_8 * cur_pid));
}










