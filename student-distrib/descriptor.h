#include "types.h"
#include "lib.h"



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
    uint32_t file_pos;
    uint32_t flags; // 0 for unused, 1 for used
  } __attribute__((packed));
} file_info;

extern file_info files[8];
extern operations file_operations;
extern operations dir_operations;
extern operations rtc_operations;
extern operations stdin_operations;
extern operations stdout_operations;


int32_t alloc_file(operations operation, int32_t inode, int32_t file_type);
int32_t free_file(int32_t file_pos);
void init_file_operations();
void init_std_op(file_info* files);
