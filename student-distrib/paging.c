/* paging.c - Implements functions to initialize page directory and page tables and enable paging
 * vim:ts=4 noexpandtab
 */

/*
   Based on modified versions of code outlined in
   https://wiki.osdev.org/Setting_Up_Paging
*/

#include "paging.h"
#include "terminal.h"

/* initialize a 4kB aligned page table with 1024 entries */
page_table_entry_t page_table[1024] __attribute__((aligned(OFFSET_4KB)));   

/* initialize a 4kB aligned page directory with 1024 entries */
page_dir_entry_t page_directory[1024] __attribute__((aligned(OFFSET_4KB))); 

/* initialize a 4kB aligned page table with 1024 entries for user space video mem mapping */
page_table_entry_t page_table_vid_mem[1024] __attribute__((aligned(OFFSET_4KB)));

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
 *  Functionality: allocates 8-12mB for task 1 and 8-12 mB for task 2 
 *  as well as a page at 128 mB for executing
 *  Arguments: task - integer that specifies which task's physical memory 
 *  the executing virtual address should map to
 *  Return: none
 */
void allocate_tasks(uint32_t task){
    /* by default, from blank_page_dir we know pages are 4 mB size already */
    /* therefore, don't need to modify pagesize field */
    /* just need to modify present bit */

    // /* set 8-12 mB present, for first task */
    // page_directory[2].present = 1;
    // /* map directly to 8mB in physical memory */
    // page_directory[2].addr_31_12_or_addr_31_22 = ((OFFSET_8MB >> 12) & KEEP_TOP10_BITS);
    
    // /* set 12-16 mB present, for second task */
    // page_directory[3].present = 1;
    // /* map directly to 12 mB in physical memory */
    // page_directory[3].addr_31_12_or_addr_31_22 = ((OFFSET_12MB >> 12) & KEEP_TOP10_BITS);

    /* set 128 mB present for execution of programs */
    page_directory[32].present = 1;
    /* map executing program to corresponding task's physical memory address */
    if(task == 1) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((OFFSET_8MB >> 12) & KEEP_TOP10_BITS);
    }else if(task == 2) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((OFFSET_12MB >> 12) & KEEP_TOP10_BITS);
    }else if(task == 3) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((137922056 >> 12) & KEEP_TOP10_BITS);
    }else if(task == 4) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((20971520 >> 12) & KEEP_TOP10_BITS);
    }else if(task == 5) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((25165824 >> 12) & KEEP_TOP10_BITS);
    }else if(task == 6) {
        page_directory[32].addr_31_12_or_addr_31_22 = ((29360128 >> 12) & KEEP_TOP10_BITS);
    }

    page_directory[32].usersupervisor = 1; // need or else page faults
    flush_tlb(); // definitely.

}

/*  add_vid_mem_page
 *  Functionality: adds a video memory page at 200MB which maps to video memory b8000-b9000
 *  Arguments: none
 *  Return: none
 */
void add_vid_mem_page() {
    /* need to specify virtual memory address that maps to video memory */
    /* video memory is 0xb8000 -> 0xb9000 */

    /* 4 MB * 256 = 1024 MB = 1 GB */
    /* arbitrarily choose 1 GB page to map to video memory */
    /* the location of this page doesn't really matter as long */
    /* as it doesn't interfere with the program image and the */
    /* kernel */
    page_directory[50].present = 1;
    page_directory[50].addr_31_12_or_addr_31_22 = ((unsigned int)page_table_vid_mem) >> 12;
    //((OFFSET_VID_MEM_START >> 12) & KEEP_TOP10_BITS);
    page_directory[50].usersupervisor = 1;
    page_directory[50].pagesize = 0; 
    int i;

    for(i = 0; i < 1024; i++) {
        // b8000 is the page for video memory
        // this is 753664 in decimal
        // 753664 / 4096 (4096 bytes = 4 kB for one page) = 184
        // so the 184th page is video memory and should be present
        // all other entries in 0-4mB should not be present
        if(i == 0) {                // VIDEO_MEMORY = 184
            page_table_vid_mem[i].present = 1;         // present
            page_table_vid_mem[i].usersupervisor = 1;
            page_table_vid_mem[i].readwrite = 1;           // read/write mode      // supervisor mode
            page_table_vid_mem[i].unused_1 = 0x00;
            page_table_vid_mem[i].accessed = 0;
            page_table_vid_mem[i].dirty = 0;
            page_table_vid_mem[i].unused_2 = 0x00;
            page_table_vid_mem[i].avail = 0x000;
            page_table_vid_mem[i].pf_addr = (OFFSET_VID_MEM_START + (i * OFFSET_4KB)) >> 12; // each page is 4 kB, no need to worry about offset since 4kB aligned
        }
        else {
            page_table_vid_mem[i].present = 0;         // present
            page_table_vid_mem[i].usersupervisor = 0;
            page_table_vid_mem[i].readwrite = 0;           // read/write mode      // supervisor mode
            page_table_vid_mem[i].unused_1 = 0x00;
            page_table_vid_mem[i].accessed = 0;
            page_table_vid_mem[i].dirty = 0;
            page_table_vid_mem[i].unused_2 = 0x00;
            page_table_vid_mem[i].avail = 0x000;
            page_table_vid_mem[i].pf_addr = (OFFSET_VID_MEM_START + (i * OFFSET_4KB)) >> 12; // each page is 4 kB, no need to worry about offset since 4kB aligned
        }  
        
    }

    /* braindead TLB flush just in case */
    flush_tlb();
}


/*  add_vid_mem_storage
 *  Functionality: adds a video memory page at 300MB, 300MB + 4KB, 300MB + 8MB which directly map to physical memory
 *  Arguments: none
 *  Return: none
 */
void add_vid_mem_storage() {
    /* 1MB, 1MB + 4KB, 1MB + 8KB pages directly mapping to physical memory */
    int i;
    for(i = 256; i <= 258; i++) {
        page_table[i].present = 1;
        page_table[i].readwrite = 1;           // read/write mode
        page_table[i].usersupervisor = 0;      // supervisor mode
        page_table[i].unused_1 = 0x00;
        page_table[i].accessed = 0;
        page_table[i].dirty = 0;
        page_table[i].unused_2 = 0x00;
        page_table[i].avail = 0x000;
        page_table[i].pf_addr = (i * OFFSET_4KB) >> 12; // each page is 4 kB, no need to worry about offset since 4kB aligned
    }
    flush_tlb();
}

/*  int32_t swap_vid_mem(int32_t terminal_number);
 *  Functionality: swaps vid mem given a terminal number
 *  Arguments:  int32_t terminal_number = terminal we want to switch to
 *  Return: 0 on success, -1 on invalid input
 */
int32_t swap_vid_mem(int32_t terminal_number) {

    if(terminal_number < 1 || terminal_number > 3) {
        return -1;
    }
    //update_cursor(terminal_array[terminal_number-1].screen_x, terminal_array[terminal_number-1].screen_y);
    //update_cursor(0, 0);
    // void* memcpy(void* dest, const void* src, uint32_t n)

    clear_key();

    int32_t* dest = (int32_t*) OFFSET_VID_MEM_START;
    int32_t* src = (int32_t*) (OFFSET_1MB + ((terminal_number - 1)*OFFSET_4KB));
    memcpy(dest, src, (uint32_t)OFFSET_4KB);

    if (terminal_array[terminal_number-1].opened_before == 0){
        terminal_array[terminal_number-1].opened_before = 1;
        clear_screen_term();
        printf_key("Starting 391 Shell\n");
        printf_key("391OS> ");
    }

    update_cursor(terminal_array[terminal_number-1].screen_x, terminal_array[terminal_number-1].screen_y);

    flush_tlb();
    /* success */
    return 0;
}

/*  
 *  Functionality: swaps video memory storage corresponding to a terminal number into actual video memory
 *  Arguments: terminal_number, can range from 1-3 inclusive
 *  Return: 0 for success, -1 if terminal_number out of bounds
 */
int32_t* get_current_vid_mem(int32_t terminal_number) {
    int32_t* cur_vid_mem_addr;
    if(terminal_number == 1) {
        cur_vid_mem_addr = (int32_t*) OFFSET_1MB;
    } else if(terminal_number == 2) {
        cur_vid_mem_addr = (int32_t*) (OFFSET_1MB + OFFSET_4KB);
    } else if(terminal_number == 3) {
        cur_vid_mem_addr = (int32_t*) (OFFSET_1MB + OFFSET_4KB + OFFSET_4KB);
    } else {
        cur_vid_mem_addr = NULL;
    }

    return cur_vid_mem_addr;
}

/*  int32_t save_vid_mem(int32_t old_terminal_num)
 *  Functionality: saves vid memory given a terminal number
 *  Arguments: int32_t old_terminal_num = terminal that we want to save vid mem from
 *  Return: 0
 */
int32_t save_vid_mem(int32_t old_terminal_num){
    int32_t* dest = (int32_t*) (OFFSET_1MB + ((old_terminal_num)*OFFSET_4KB));
    int32_t* src = (int32_t*) OFFSET_VID_MEM_START; 
    memcpy(dest, src, (uint32_t)OFFSET_4KB);

    return 0;
}


