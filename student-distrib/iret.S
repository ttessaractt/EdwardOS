

.text
    .globl jump_to_user

# switch to user 
jump_to_user:
    pushl %ebp
    movl %esp, %ebp

    # put iret context onto the stack
    push $0x2B
    push $0x83ffffc
    pushf
    push $0x23

    # put first parameter on stack (EIP/entrypoint into program)
    pushl 8(%ebp)
    iret
