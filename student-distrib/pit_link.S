
#define ASM 1
.text  
    .globl PIT_link

PIT_link:
    pushal
    pushfl          # push flags
    call PIT_handler
    popfl
    popal
    iret

