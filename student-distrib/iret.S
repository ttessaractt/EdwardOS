#define USER_DS 0x002B
#define USER_CS 0x0023

.text
    .globl jump_to_user

jump_to_user:
    pushl %ebp
    movl %esp, %ebp
    # prepare for context switch
    movw USER_DS, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    # put iret context onto the stack
    movl %esp, %eax
    push USER_DS
    push %eax
    pushf
    push USER_CS

    # put first parameter on stack (EIP/entrypoint into program)
    pushl 8(%ebp)
    iret
