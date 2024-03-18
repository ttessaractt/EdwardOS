#include "types.h"

extern void set_page_table();
extern void blank_page_dir();

extern void enable_paging();
extern void enable_4mb_pages();

typedef union page_table_entry_t {
    uint32_t val;
    struct {
        uint32_t present : 1;
        uint32_t readwrite: 1;
        uint32_t usersupervisor: 1;
        uint32_t unused_1: 2;
        uint32_t accessed: 1;
        uint32_t dirty: 1;
        uint32_t unused_2: 2;
        uint32_t avail: 3;
        uint32_t pf_addr: 20;
    } __attribute__ ((packed));
} page_table_entry_t;

// typedef union page_table_entry_4mb_t {
//     uint32_t val;
//     struct {
//         uint32_t present : 1;
//         uint32_t readwrite: 1;
//         uint32_t usersupervisor: 1;
//         uint32_t unused_1: 2;
//         uint32_t accessed: 1;
//         uint32_t dirty: 1;
//         uint32_t pagesize: 1;
//         uint32_t unused_2: 1;
//         uint32_t avail: 3;
//         uint32_t pat_unused: 1;
//         uint32_t addr_39_32: 8;
//         uint32_t rsvd_unused: 1;
//         uint32_t addr_31_22: 10;
//     } __attribute__ ((packed));
// } page_table_entry_4mb_t;


typedef union page_dir_entry_t {
    uint32_t val;
    struct {
        uint32_t present : 1;
        uint32_t readwrite: 1;
        uint32_t usersupervisor: 1;
        uint32_t unused_pwt: 1;
        uint32_t unused_pcd: 1;
        uint32_t accessed: 1;
        uint32_t unused_dirty: 1;
        uint32_t pagesize: 1;
        uint32_t unused_global: 1;
        uint32_t avail: 3;
        uint32_t addr_31_12_or_addr_31_22: 20;
    } __attribute__ ((packed));
} page_dir_entry_t;



extern void load_page_dir(page_dir_entry_t* page_dir_base);

extern page_table_entry_t page_table[1024] __attribute__((aligned(4096)));
//extern page_table_entry_4mb_t page_table_kernel[1024] __attribute__((aligned(4096)));
extern page_dir_entry_t page_directory[1024] __attribute__((aligned(4096)));

