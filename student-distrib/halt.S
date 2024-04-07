
.text
    .global halt_asm

halt_asm:
    pushl %ebp
    movl %esp, %ebp


    movl 12(%ebp), %eax 
    movl 8(%ebp), %ecx
    movl %ecx, %ebp

   

    leave
    ret
