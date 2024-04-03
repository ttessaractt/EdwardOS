# set_paging.S - sets control registers appropriately in the IA-32 in order to enable paging
# vim:ts=4 noexpandtab

/*
   Based on modified versions of code outlined in
   https://wiki.osdev.org/Setting_Up_Paging
*/

# definitions for setting special bits in control registers
#define BIT31_BIT0 0x80000001 
#define BIT4       0x00000010

.text
    .globl load_page_dir
    .globl enable_paging
    .globl enable_4mb_pages
    .globl flush_tlb

/*  void load_page_dir(page_dir_entry_t* page_dir_base)
 *  Functionality: loads the directory of the page directory into CR3
 *  so the CPU knows where to start accessing memory
 *  Arguments: page_dir_base - a pointer to the start of the page directory
 *  Return: none
 */
load_page_dir:
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    movl 8(%ebp), %eax    # moves page_dir_base pointer into eax
    movl %eax, %cr3       # loads page_dir_base pointer into CR3
    popl %eax
    leave
    ret

/*  void enable_paging()
 *  Functionality: enables paging on the IA-32 by setting bit 31 of CR0
 *  and enabling protected mode by setting bit 0 of CR0
 *  Arguments: none
 *  Return: none
 */
enable_paging:
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    movl %cr0, %eax       # temporarily move the value of CR0 into eax
    orl $BIT31_BIT0, %eax # set bit 31, the paging bit, to 1 in CR0 register and CR0 bit 0 to 1 to enable protected mode
    movl %eax, %cr0       # move modified value back into CR4
    popl %eax
    leave
    ret

/*  void enable_4mb_pages()
 *  Functionality: enables the mixed use of 4mB and 4kB pages by
 *  setting bit 4 of CR4
 *  Arguments: none
 *  Return: none
 */
enable_4mb_pages:
    # set PSE flag in CR4 to enable mixing 4kB and 4mB pages
    # PSE flag is bit 4
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    movl %cr4, %eax       # temporarily move the value of cr4 into eax
    orl $BIT4, %eax # set bit 4 in CR4 to enable 4mB paging
    movl %eax, %cr4       # move modified value back into CR4
    popl %eax
    leave
    ret

/*  void flush_tlb()
 *  Functionality: flushes the TLB by writing to CR3
 *  Arguments: none
 *  Return: none
 */
flush_tlb:
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    movl %cr3, %eax # move cr3 into eax (to maintain physical address of page directory structure)
    movl %eax, %cr3 # move eax back into cr3, flushes tlb
    popl %eax
    leave
    ret


