#include "types.h"
#include "lib.h"

typedef union dentry_t {
  struct {
    int8_t file_name[33];
    uint32_t file_type;
    uint32_t inode_num;
    char reserved[24]; 
  } __attribute__((packed));
} dentry_t;

typedef union boot_block {
  struct {
    uint32_t num_dentries;
    uint32_t num_inodes;
    uint32_t data_blocks;
    char reserved[52];
    dentry_t dentries[63];  
  } __attribute__((packed));
} boot_block;

typedef union inode {
  struct {
    int32_t length;
    int32_t data_block[1023];
  } __attribute__((packed));
} inode;

typedef union data_block {
  struct {
    int8_t data[10000];
  } __attribute__((packed));
} data_block;

extern dentry_t cur_file;
extern inode cur_file_det;
extern data_block data_buffer;

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
