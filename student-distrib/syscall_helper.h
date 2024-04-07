/* syscall_helper.h - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
// #include "descriptor.h"

/* OPERATIONS */

typedef union operations {
  struct {
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open) (const uint8_t* filename);
    int32_t (*close) (int32_t fd);
  } __attribute__((packed));
} operations;

typedef union file_info {
  struct {
    operations fotp; //files operation table pointer
    uint32_t inode;
    int32_t file_pos;
    uint32_t flags; // 0 for unused, 1 for used
  } __attribute__((packed));
} file_info;


typedef struct process_control_block_t{
    int32_t pid;
    int32_t parent_pid;

    uint32_t esp;
    uint32_t tss_esp0;
    uint32_t ebp;

    //struct process_control_block_t * next;
    //struct process_control_block_t * parent;

    /* need file discriptor array */
    file_info file_d_array[8]; //need to make it!!

    //signal - add later

} process_control_block_t; 

//process_control_block_t PCB_1;
//process_control_block_t PCB_2;

int32_t current_pid;
int32_t current_parent_pid;
process_control_block_t* current_process;

int32_t execute_help(unsigned char* command);

int32_t halt_help(unsigned char status);

/* need to pass in file name and argument buffer of proper size */
int32_t parse_arguments(unsigned char* buf, unsigned char* file_name, unsigned char* arguments);

int32_t initialize_pcb(unsigned char* file_name);


// extern file_info files[8];
extern operations file_operations;
extern operations dir_operations;
extern operations rtc_operations;
extern operations stdin_operations;
extern operations stdout_operations;


int32_t alloc_file(operations operation, int32_t inode, int32_t file_type, file_info* files);
int32_t free_file(int32_t file_pos, file_info* files);
void init_file_operations();
void init_std_op(file_info* files);
void init_zero(file_info* files);
