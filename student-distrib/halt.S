
.text
    .global halt_asm

# halt assembly code
halt_asm:
    pushl %ebp
    movl %esp, %ebp
    # move status (return value to parent program) into eax
    movl 12(%ebp), %eax 
    # move saved ebp (from execute) into ebp
    movl 8(%ebp), %ecx
    movl %ecx, %ebp

    leave
    ret
