
.text
    .global halt_asm

halt_asm:
    pushl %ebp
    movl %esp, %ebp
    
    movl 8(%ebp), %ecx 
    movl 12(%ebp), %edx

    movl %ecx, %ebp
    movl %ecx, %esp
    
    movl %edx, %eax

    # leave
    ret
