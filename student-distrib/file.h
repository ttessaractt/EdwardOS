#include "types.h"
#include "lib.h"

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
    int8_t data[40000];
  } __attribute__((packed));
} data_block;

/* global variables that store state of current file */
extern dentry_t cur_file;
extern inode cur_file_det;
extern data_block data_buffer;
extern unsigned int num_dir_entries;
extern dentry_t cur_dir;
extern uint32_t dentry_index;
extern uint32_t file_size;

uint32_t file_open();
uint32_t file_close();
uint32_t file_read();
uint32_t file_write();

uint32_t directory_open();
uint32_t directory_close();
uint32_t directory_read();
uint32_t directory_write();

uint32_t read_dentry_by_name(const int8_t* fname, dentry_t* dentry);
uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
uint32_t read_data(uint32_t inode, uint32_t offset, int8_t* buf, uint32_t length);
uint32_t file_key_write(uint32_t fd, char* buf, uint32_t nbytes);
uint32_t check_file_validity(int8_t* fname);

