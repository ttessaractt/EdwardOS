#include "signals.h"
#include "ece391sigtest.c"

sigaction signals[5];
/*
Signal Name     Signal Number   Default Action

DIV_ZERO        0               Kill the task
SEGFAULT        1               Kill the task
INTERRUPT       2               Kill the task
ALARM           3               Ignore
USER1           4               Ignore
*/

void div_zer_def(int32_t signum){

}

void segfault_def(int32_t signum){
    segfault_sighandler(signum);
}

void interrupt_def(int32_t signum){

}

void alarm_def(int32_t signum){

}

void user1_def(int32_t signum){

}

void init_def_signals(){
    signals[0].sa_handler = div_zer_def;
    signals[1].sa_handler = segfault_def;
    signals[2].sa_handler = interrupt_def;
    signals[3].sa_handler = alarm_def;
    signals[4].sa_handler = user1_def;
}

void set_handler_helper(int32_t signum, void* handler){
    signals[signum].sa_handler = handler;
}


