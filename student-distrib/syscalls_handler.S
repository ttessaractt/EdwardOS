#syscalls.S - 

#define ASM 1

.text  
    .globl syscall_handler

syscall_handler:
    # don't save and restore EAX, will have return value (if there is one)
    # save regs
    pushl %ebp  
    pushl %esi
    pushl %edi
    pushl %esp

    # value checking, check if call number in eax is between 1 & 10
    cmpl $1, %eax        # eax < 1?
    jb syscall_error      # eax is < 1, error
    cmpl $10, %eax       # eax > 10?
    ja syscall_error      # eax is > 10, error

    # push function argurmnts
    pushl %edx
    pushl %ecx
    pushl %ebx   # 1st arguemnt

    # call syscall
    call *syscall_table(,%eax,4)    # call system call using syscall jumptable
    popl %ebx
    popl %ecx
    popl %edx
    jmp syscall_done

syscall_error:          # for value checking
    movl $-1, %eax      # error in call number, return -1

syscall_done:           # return from system call, either call is finished or value not valid
    # return regs
    popl %esp
    popl %edi
    popl %esi
    popl %ebp
    iret

syscall_table:      # jump table of the syscalls
    .long halt, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
