#include "types.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"

#define BLOCK_LENGTH 4096

/* size is in bytes */

#define MAX_NUM_OF_DIR_ENTRIES 63
#define MAX_NUM_OF_FILES 62

#define SYS_STATISTICS_SIZE 64
#define NUM_DIR_ENTRIES_SIZE 4
#define NUM_I_NODES_SIZE 4
#define NUM_DATA_BLOCKS_SIZE 4

#define DENTRY_SIZE 64
#define FILE_NAME_SIZE 32
#define FILE_TYPE_SIZE 4
#define I_NODE_NUM_SIZE 4

#define LENGTH_IN_BYTES_SIZE 4
#define DATA_BLOCK_INDEX_SIZE 4

#define MB_8 0x800000
#define KB_8 0x2000
#define SPACE_CHAR 0x20
#define MAX_FILE_NAME_LENGTH 32
#define MAX_ARG_LENGTH 128
#define EXCEPTION 256
#define FD_ARRAY_LEN 8
#define FIRST_FILE_OFFSET 2
#define NORMAL_FILE_NUM 2

#define FOUR 4
#define MAX 255

/* dentry structure */
typedef union dentry_t {
  struct {
    int8_t file_name[33];
    uint32_t file_type;
    uint32_t inode_num;
    char reserved[24]; 
  } __attribute__((packed));
} dentry_t;

/* boot block structure */
typedef union boot_block {
  struct {
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t data_blocks;
    char reserved[52];
    dentry_t dentries[63];  
  } __attribute__((packed));
} boot_block;

/* inode structure */
typedef union inode {
  struct {
    int32_t length;
    int32_t data_block[1023];
  } __attribute__((packed));
} inode;

/* buffer to hold data */
typedef union data_block {
  struct {
    int8_t data[4096];
  } __attribute__((packed));
} data_block;

typedef void (*jump)(void);

/* global variables that store state of current file */
extern dentry_t cur_file;
extern inode cur_file_det;
extern data_block data_buffer;
extern unsigned int num_dir_entries;
extern dentry_t cur_dir;
extern int32_t dentry_index;
extern uint32_t file_size;
//extern int32_t current_pid;

int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);

int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t directory_open(const uint8_t* filename);
int32_t directory_close(int32_t fd);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length); // added fd
int32_t file_key_write(uint32_t fd, char* buf, uint32_t nbytes);
int32_t check_file_validity(uint8_t* fname, dentry_t* dentry);





/* OPERATIONS */
typedef union operations {
  struct {
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open) (const uint8_t* filename);
    int32_t (*close) (int32_t fd);
  } __attribute__((packed));
} operations;

/*holer for file information*/
typedef union file_info {
  struct {
    operations fotp; //files operation table pointer
    uint32_t inode;
    int32_t file_pos;
    uint32_t flags; // 0 for unused, 1 for used
  } __attribute__((packed));
} file_info;

/*holder for PCB information*/
typedef struct process_control_block_t{
    int32_t pid;
    int32_t parent_pid;

    uint32_t esp;
    uint32_t tss_esp0;
    uint32_t ebp;
    uint32_t ebp_switch;
    uint32_t base_shell;
    //arguments for getargs
    unsigned char arguments[1024];       
    uint32_t arg_length;

    dentry_t cur_file_dentry;

    int32_t terminal_id;

    /* need file discriptor array */
    file_info file_d_array[8]; //need to make it!!

    //signal - add later

} process_control_block_t; 

//process_control_block_t PCB_1;
//process_control_block_t PCB_2;

int32_t GOD;
int32_t max_programs_flag;

int32_t current_pid;
int32_t current_parent_pid;
process_control_block_t* current_process;

/*function that actually does execute*/
int32_t execute_help(unsigned char* command);

/*function that actually does halt*/
int32_t halt_help(unsigned char status);

/*function to parse the inputs in execute*/
int32_t parse_arguments(unsigned char* buf, unsigned char* file_name, unsigned char* arguments);

/*function to initilize the PCB in execute*/
int32_t initialize_pcb(void);

/*function to actually do vidmap*/
int32_t vidmap_helper(uint8_t** screen_start);

int32_t calculate_pcb_addr(int32_t cur_pid);

/*function that actually does getargs*/
int32_t getargs_helper(uint8_t* buf, int32_t nbytes);

/* helper function to find next pid*/
int32_t find_next_pid();

/* helper function to check if current process is a base shell */
int32_t is_base_shell();

// extern file_info files[8];
extern operations file_operations;
extern operations dir_operations;
extern operations rtc_operations;
extern operations stdin_operations;
extern operations stdout_operations;


int32_t alloc_file(operations operation, int32_t inode, int32_t file_type, file_info* files);
int32_t free_file(file_info* files);
void init_file_operations();
void init_std_op(file_info* files);
void init_zero(file_info* files);
