#include "paging.h"

page_table_entry_4kb_t page_table[1024] __attribute__((aligned(4096)));
page_table_entry_4mb_t page_table_kernel[1024] __attribute__((aligned(4096)));
page_dir_entry_t page_directory[1024] __attribute__((aligned(4096)));

void set_page_table(){
    unsigned int i;
    for(i = 0; i < 1024; i++) {
        // b8000 is the page for video memory
        // this is 753664 in decimal
        // 753664 / 4096 (4096 bytes = 4 kB for one page) = 184
        // so the 184th page is video memory and should be present
        if(i == 184) {
            page_table[i].present = 1;         // present
        }
        else {
            page_table[i].present = 0;         // not present
        }  
        page_table[i].readwrite = 1;       // read/write
        page_table[i].usersupervisor = 0;  // supervisor mode
        page_table[i].unused_1 = 0;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].pagesize = 0; // 4 kB size page
        page_table[i].unused_2 = 0;
        page_table[i].avail = 0;
        page_table[i].pf_addr = (i * 4096) >> 12; // each page is 4 kB, no need to worry about offset since 4kB aligned
    }

    for(i = 0; i < 1024; i++) {
        page_table_kernel[i].present = 1; 
        page_table_kernel[i].readwrite = 1;       // read/write
        page_table_kernel[i].usersupervisor = 0;  // supervisor mode
        page_table_kernel[i].unused_1 = 0;
        page_table_kernel[i].accessed = 0;
        page_table_kernel[i].dirty = 0;
        page_table_kernel[i].pagesize = 1; // 4 mB size page
        page_table_kernel[i].unused_2 = 0;
        page_table_kernel[i].avail = 0;
        page_table_kernel[i].pat_unused = 0;
        page_table_kernel[i].rsvd_unused = 0;
        uint32_t addr_shifted = (4194304 + (i * 4096)) >> 20; // recover top 18 bits
        // bottom 10 bits are 31-22, top 8 bits are 39-32
        page_table_kernel[i].addr_39_32 = addr_shifted >> 10; // recover top 8 bits 
        page_table_kernel[i].addr_31_22 = addr_shifted & 0x000003FF; // recover bottom 10 bits
    }
    // first 4 mB (0 - 4 mB)
    page_directory[0].present = 1;         // present
    page_directory[0].readwrite = 1;       // read/write
    page_directory[0].usersupervisor = 0;  // supervisor mode
    page_directory[0].unused_1 = 0;
    page_directory[0].accessed = 0;
    page_directory[0].dirty = 0;
    page_directory[0].pagesize = 0;
    page_directory[0].unused_2 = 0;
    page_directory[0].avail = 0;
    page_directory[0].pf_addr = ((unsigned int)page_table) >> 12;  // 12 shift fixed bootloop

    // second 4 mB (4 - 8 mB) (kernel)
    page_directory[1].present = 1;         // present
    page_directory[1].readwrite = 1;       // read only
    page_directory[1].usersupervisor = 0;  // supervisor mode
    page_directory[1].unused_1 = 0;
    page_directory[1].accessed = 0;
    page_directory[1].dirty = 0;
    page_directory[1].pagesize = 1;  // 4 mB size page
    page_directory[1].unused_2 = 0;
    page_directory[1].avail = 0;
    page_directory[1].pf_addr = ((unsigned int)page_table_kernel) >> 12; // 12 shift fixed bootloop
}

void blank_page_dir(){
    int i;
    for(i = 0; i < 1024; i++) {
        // 0x00000002
        // 0000 0000 0000 0000 0000 0000 0000 0010
        page_directory[i].present = 0;         // not present
        page_directory[i].readwrite = 1;       // read/write
        page_directory[i].usersupervisor = 0;  // supervisor mode
        page_directory[i].unused_1 = 0;
        page_directory[i].accessed = 0;
        page_directory[i].dirty = 0;
        page_directory[i].pagesize = 0;
        page_directory[i].unused_2 = 0;
        page_directory[i].avail = 0;
        page_directory[i].pf_addr = 0;
    }
}

