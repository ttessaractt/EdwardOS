#define ASM 1

#define LINK(name, func)    \
    .global name        ;\
    name:               ;\
        pushal          ;\
        pushfl          ;\
        call func       ;\
        popfl           ;\
        popal           ;\
        iret

LINK(rtc_handler_linkage, RTC_handler);
LINK(key_handler_linkage, keyboard_handler);