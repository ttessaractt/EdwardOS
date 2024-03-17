#define ASM 1

#define INTR_LINK(name, func)    \
    .global name        ;\
    name:               ;\
        pushal          ;\
        pushfl          ;\
        call func       ;\
        popfl           ;\
        popal           ;\
        iret

INTR_LINK(rtc_handler_linkage, RTC_handler);
