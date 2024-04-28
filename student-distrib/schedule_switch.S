
#define ASM 1
.text  
    .globl schedule_switch

schedule_switch:
    pushl %ebp
    movl %esp, %ebp
    # move saved ebp (from switch) into ebp
    movl 8(%ebp), %ecx
    movl %ecx, %ebp
    # put first parameter on stack (EIP/entrypoint into program)
    leave
    ret

