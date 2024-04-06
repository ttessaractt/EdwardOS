#syscalls.S - 

#define ASM 1

.text  
    .globl syscall_handler

syscall_handler:
    # don't save and restore EAX, will have return value (if there is one)
    # save regs
    # gf pushf
    pushl %ebp  
    pushl %esi
    pushl %edi
    pushl %esp
    
    # push function argurmnts
    pushl %edx
    pushl %ecx
    pushl %ebx   # 1st arguemnt

    # value checking, check if call number in eax is between 1 & 10
    # when eax < 1
    cmpl $1, %eax        # eax < 1?
    jl syscall_error     # eax is < 1, error
    # when eax > 10
    cmpl $10, %eax       # eax > 10?
    jg syscall_error     # eax is > 10, error

    # call syscall
    call *syscall_table(,%eax,4)    # call system call using syscall jumptable
    popl %ebx
    popl %ecx
    popl %edx
    movl $0, %eax       # no error so return 0
    jmp syscall_done

syscall_error:
    movl $-1, %eax

syscall_done:       # return from system call, either call is finished or value not valid
    # return regs
    popl %esp
    popl %edi
    popl %esi
    popl %ebp
    iret

syscall_table:      # jump table of the syscalls
    .long halt, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
