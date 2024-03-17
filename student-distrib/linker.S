
#define ASM     1

#define LINK(name, func) \
    .global name            ;\
    name:                   ;\
        pushal              ;\
        pushfl              ;\
        call func           ;\
        popfl               ;\
        popal               ;\
        iret                

LINK(key_handler_linkage, keyboard_handler);
# LINK(rtc_handler_linkage, rtc_handler);