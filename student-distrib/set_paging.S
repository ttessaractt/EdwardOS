.text
    .globl load_page_dir
    .globl enable_paging
    .globl enable_4mb_pages

load_page_dir:
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    movl 8(%ebp), %eax
    movl %eax, %cr3       # loads address of page directory into CR3
    popl %eax
    leave
    ret

enable_paging:
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    movl %cr0, %eax
    orl $0x80000001, %eax # set 32nd bit, the paging bit, to 1 in CR0 register and CR0 bit 0 to enable protected mode
    movl %eax, %cr0
    popl %eax
    leave
    ret

enable_4mb_pages:
    # set PSE flag in CR4 to enable mixing 4kB and 4mB pages
    # PSE flag is bit 4
    pushl %ebp
    movl %esp, %ebp
    pushl %eax
    movl %cr4, %eax
    orl $0x00000010, %eax
    movl %eax, %cr4
    popl %eax
    leave
    ret


