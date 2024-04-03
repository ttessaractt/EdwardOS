#include "types.h"
#include "file.h"
void program_loader(int8_t* file_name, int32_t task_number) {

    /* check for appropriate task number */
    if(task_number < 1 || task_number > 2) {
        return;
    }

    int8_t* cur_addr;

    /* opens file and copies file data to data_buffer */
    file_open(file_name);
    file_read(file_name);

    /* choose where in physical memory to copy file data */
    if(task_number == 1) {
        cur_addr = (int8_t*)0x0800000; // 8 mB
    } else if(task_number == 2) {
        cur_addr = (int8_t*)0x0C00000; // 12 mB
    }

    int32_t i;
    for(i = 0; i < 40000; i++) { // 1024 * 1024 * 4 = number of bytes in 4 mB
        *cur_addr = data_buffer.data[i];
        cur_addr = cur_addr + 1;
    }
}

