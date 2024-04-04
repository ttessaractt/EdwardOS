#include "types.h"
#include "lib.h"

typedef union file_info {
  struct {
    uint32_t fotp; //files operation table pointer
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags; // 0 for unused, 1 for used
  } __attribute__((packed));
} file_info;




uint32_t alloc_file(uint32_t fotp, uint32_t inode, uint32_t file_type);
uint32_t free_file(uint32_t file_pos);
