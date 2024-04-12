#include "types.h"

/*
Signal Name     Signal Number   Default Action

DIV_ZERO        0               Kill the task
SEGFAULT        1               Kill the task
INTERRUPT       2               Kill the task
ALARM           3               Ignore
USER1           4               Ignore
*/

typedef union sigaction {
    struct{
       void (*sa_handler) (int32_t); 
    } __attribute__((packed));
} sigaction;

void div_zer_def(int32_t signum);
void segfault_def(int32_t signum);
void interrupt_def(int32_t signum);
void alarm_def(int32_t signum);
void user1_def(int32_t signum);
void init_def_signals();
void set_handler_helper(int32_t signum, void* handler);
 