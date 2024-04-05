#include "types.h"
#include "lib.h"
#

typedef union file_info {
  struct {
    uint32_t fotp; //files operation table pointer
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags; // 0 for unused, 1 for used
  } __attribute__((packed));
} file_info;




int32_t alloc_file(operations* operation, int32_t inode, int32_t file_type);
int32_t free_file(int32_t file_pos);
