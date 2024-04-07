
.text
    .global halt_asm

halt_asm:
    pushl %ebp
    movl %esp, %ebp

    addl $8, %esp
    
    popl %ebp
    movl %ebp, %esp

    pushl %ebx

    ret
