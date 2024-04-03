/* paging.c - Implements functions to initialize page directory and page tables and enable paging
 * vim:ts=4 noexpandtab
 */

/*
   Based on modified versions of code outlined in
   https://wiki.osdev.org/Setting_Up_Paging
*/

#include "paging.h"

/* initialize a 4kB aligned page table with 1024 entries */
page_table_entry_t page_table[1024] __attribute__((aligned(OFFSET_4KB)));   

/* initialize a 4kB aligned page directory with 1024 entries */
page_dir_entry_t page_directory[1024] __attribute__((aligned(OFFSET_4KB))); 

/* set_page_table
 *  Functionality: sets the video memory range 0xB8000 - 0xB9000 to be present in the page table
 *  and also sets the page directory have entries pointing to the page table and the 4mB page
 *  for the kernel
 *  Arguments: none
 *  Return: none
 */
void set_page_table(){
    unsigned int i;
    // set the 0-4mB page table
    for(i = 0; i < 1024; i++) {
        // b8000 is the page for video memory
        // this is 753664 in decimal
        // 753664 / 4096 (4096 bytes = 4 kB for one page) = 184
        // so the 184th page is video memory and should be present
        // all other entries in 0-4mB should not be present
        if(i == VIDEO_MEMORY) {                // VIDEO_MEMORY = 184
            page_table[i].present = 1;         // present
        }
        else {
            page_table[i].present = 0;         // not present
        }  
        page_table[i].readwrite = 1;           // read/write mode
        page_table[i].usersupervisor = 0;      // supervisor mode
        page_table[i].unused_1 = 0x00;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].unused_2 = 0x00;
        page_table[i].avail = 0x000;
        page_table[i].pf_addr = (i * OFFSET_4KB) >> 12; // each page is 4 kB, no need to worry about offset since 4kB aligned
    }

    // Make the first entry of the page directory point to the 4kB/entry page table
    // Represents the first 4 mB (0 - 4 mB)
    page_directory[0].present = 1;         // present
    page_directory[0].readwrite = 1;       // read/write mode
    page_directory[0].usersupervisor = 0;  // supervisor mode
    page_directory[0].unused_pwt = 0;
    page_directory[0].unused_pcd = 0;
    page_directory[0].accessed = 0;
    page_directory[0].unused_dirty = 0;
    page_directory[0].pagesize = 0;        // 4kB page size
    page_directory[0].unused_global = 0;
    page_directory[0].avail = 0;
    // shift by 12 to recover top 20 bits for address
    page_directory[0].addr_31_12_or_addr_31_22 = ((unsigned int)page_table) >> 12; 

    // Second 4 mB (4 - 8 mB) (kernel)
    page_directory[1].present = 1;         // present
    page_directory[1].readwrite = 1;       // read/write mode
    page_directory[1].usersupervisor = 0;  // supervisor mode
    page_directory[1].unused_pwt = 0;
    page_directory[1].unused_pcd = 0;
    page_directory[1].accessed = 0;
    page_directory[1].unused_dirty = 0;
    page_directory[1].pagesize = 1;        // 4mB size page
    page_directory[1].unused_global = 0;
    page_directory[1].avail = 0;
    // directly link the second entry to the kernel page which is 4mB
    // shift by 12 to recover top 20 bits
    // use mask to zero out bottom of those 20 bits since in 4mB entries
    // these bits will be unused
    page_directory[1].addr_31_12_or_addr_31_22 = ((OFFSET_4MB >> 12) & KEEP_TOP10_BITS); 
}

/*  blank_page_dir
 *  Functionality: initializes the page directory to contain blank/not present 
 *  entries inside the 0-4gB range
 *  Arguments: none
 *  Return: none
 */
void blank_page_dir(){
    int i;
    for(i = 0; i < 1024; i++) {
        page_directory[i].present = 0;         // not present
        page_directory[i].readwrite = 1;       // read/write mode
        page_directory[i].usersupervisor = 0;  // supervisor mode
        page_directory[i].unused_pwt = 0;
        page_directory[i].unused_pcd = 0;
        page_directory[i].accessed = 0;
        page_directory[i].unused_dirty = 0;
        page_directory[i].pagesize = 1;        // 4 mB size page by default, so 8mB - 4gB is all not present
        page_directory[i].unused_global = 0;
        page_directory[i].avail = 0;
        page_directory[i].addr_31_12_or_addr_31_22 = 0;
    }
}

/*  allocate_tasks
 *  Functionality: allocates 8-12mB for task 1 and 12-16 mB for task 2 
 *  as well as a page at 128 mB for executing
 *  Arguments: task - integer that specifies which task's physical memory 
 *  the executing virtual address should map to
 *  Return: none
 */
void allocate_tasks(uint32_t task){
    /* by default, from blank_page_dir we know pages are 4 mB size already */
    /* therefore, don't need to modify pagesize field */
    /* just need to modify present bit */

    /* set 8-12 mB present, for first task */
    page_directory[2].present = 1;
    /* map directly to 8mB in physical memory */
    page_directory[2].addr_31_12_or_addr_31_22 = ((OFFSET_8MB >> 12) & KEEP_TOP10_BITS);

    /* set 12-16 mB present, for second task */
    page_directory[3].present = 1;
    /* map directly to 12 mB in physical memory */
    page_directory[3].addr_31_12_or_addr_31_22 = ((OFFSET_12MB >> 12) & KEEP_TOP10_BITS);

    /* set 128 mB present for execution of programs */
    page_directory[32].present = 1;
    /* map executing program to corresponding task's physical memory address */
    if(task == 1) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((OFFSET_8MB >> 12) & KEEP_TOP10_BITS);
    } else if(task == 2) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((OFFSET_12MB >> 12) & KEEP_TOP10_BITS);
    }
}

