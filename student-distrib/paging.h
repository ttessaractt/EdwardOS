/* paging.h - Defines functions to initialize page directory and page tables and enable paging
 * as well as important structs to easily change important bits in table/directory entries
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "lib.h"

// important magic numbers for operations in paging.c
#define VIDEO_MEMORY 184
#define OFFSET_4KB 4096
#define OFFSET_1MB 0x100000
#define OFFSET_4MB 4194304
#define OFFSET_8MB 8388608
#define OFFSET_12MB 12582912
#define OFFSET_128MB 0x8000000
#define OFFSET_1GB 0x40000000
#define OFFSET_200 0xC800000
#define OFFSET_VID_MEM_START 0xB8000
#define OFFSET_VID_MEM_END 0xB9000
#define KEEP_TOP10_BITS 0xFFC00

/* sets the 0-4mB page table and links the page directory to this page table and the 4mB kernel page */
extern void set_page_table();

/* sets the 0-4gB range as all non-present by default */
extern void blank_page_dir();

/* sets up the 8-16 mB range to hold two tasks, with a 4mB page at 128 mB for execution */
extern void allocate_tasks(uint32_t task);

/* enables paging in the IA-32 */
extern void enable_paging();

/* enables 4mB pages to be used */
extern void enable_4mb_pages();

/* adds a virtual page that maps to video memory so user space can access it */
extern void add_vid_mem_page();

/* adds three virtual pages that map directly to their corresponding physical address */
extern void add_vid_mem_storage();

/* swaps one page to point to actual vid mem and others to point to physical */
extern int32_t swap_vid_mem(int32_t terminal_number);

/* returns the virtual address of the page that is currently pointing to actual video memory */
extern int32_t* get_current_vid_mem(int32_t terminal_number); 

extern int32_t save_vid_mem(int32_t old_terminal_num);

/* struct defining bit correspondence for page table entries */
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

/* struct defining bit correspondence for page directory entries */
/* addressing is different based on if the entry is 4kB or 4mB */
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
        /* if entry is 4kB addr is bits 31-12 */
        /* if entry is 4mB, addr is bits 31-22 and bits 21-12 are unused */
        uint32_t addr_31_12_or_addr_31_22: 20;
    } __attribute__ ((packed));
} page_dir_entry_t;

/* loads a pointer to the page directory starting address into CR3 */
extern void load_page_dir(page_dir_entry_t* page_dir_base);

/* define the structures for the 0-4mB page table and the page directory */
extern page_table_entry_t page_table[1024] __attribute__((aligned(OFFSET_4KB)));
extern page_dir_entry_t page_directory[1024] __attribute__((aligned(OFFSET_4KB)));
extern page_table_entry_t page_table_vid_mem[1024] __attribute__((aligned(OFFSET_4KB)));

extern void flush_tlb();
