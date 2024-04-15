#include "types.h"
#include "file.h"
#include "kernel.h"
#include "loader.h"

/* program_loader
 * Description: helper function that copies file data into the start of program memory
 * Inputs: file_name - name of the file
 *         task_number - usually the current pid which describes which process is running
 *         dentry - dentry with program metadata
 * Return Value: none
 */
void program_loader(int8_t* file_name, int32_t task_number, dentry_t* dentry) {
    /* check for appropriate task number */
    if(task_number < 1 || task_number > 2) {
        return;
    }

    /* calculate inode_addr */
    int8_t* inode_addr = (int8_t*) boot_block_addr + BLOCK_LENGTH + 
        (dentry->inode_num * BLOCK_LENGTH);
    
    /* initialize length to 0 */
    int32_t length = 0;

    /* copy length of file into length variable*/
    memcpy(&length, inode_addr, 4);

    /* call read_data on program start address to copy data there */
    read_data(dentry->inode_num, 0, (int8_t*)PROGRAM_START, length); // read entire file data
}

