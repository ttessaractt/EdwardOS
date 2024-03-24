#include "ece391support.h"
#include "ece391syscall.h"
#include "ece391sysnum.h"

int32_t file_open();
int32_t file_close();
int32_t file_read();
int32_t file_read();

int32_t directory_open();
int32_t directory_close();
int32_t directory_read();
int32_t directory_write();

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
  

struct boot_block {
  int32_t num_dentries;
  int32_t inodes;
  int32_t data_blocks;
  char reserved[52];
  
} __attribute((packed));

struct dentry_t {
  char file_name[32];
  int32_t file_type;
  int32_t inode_num;
  char reserved[24];
} __attribute((packed));

struct inode {
  int32_t length;
  int32_t data_block[15];
} __attribute((packed));