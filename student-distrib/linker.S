# linker.S - links device handlers to IDT (for when interrupt called)
# vim:ts=4 noexpandtab

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

