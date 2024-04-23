#include "types.h"
#include "i8259.h"
#include "rtc.h"
#include "lib.h"
#include "terminal.h"
#include "file.h"
#include "x86_desc.h"
#include "schedule_switch.h"
#include "paging.h"

int volatile pit_interrupt_occured = 0;
int active_processes[3] = {0, 0, 0}; 

/*
https://wiki.osdev.org/Programmable_Interval_Timer#Mode_0_.E2.80.93_Interrupt_On_Terminal_Count
*/
void PIT_init(){
    //0x43 - Mode/Command register (write only, a read is ignored)
    //00110000 = 0x1ADB0
    //set PIT mode/command register
    //printf("init\n");
    outb(0x43, 0x1ADB0);     //theoretically set pit mode/command register
    
    //set low&high byte of reload value
    //time in ms = reload_value * 3000 / 3579545
    //10ms = 0x9C2E, 0x9C & (0x2E00)>>8
	outb(0x40, 0x9C);		// Low byte
	outb(0x40, (0x2E00)>>8);	// High byte
    
    //enable IRQ0 on PIC
    enable_irq(0);          //enable PIT interrupt on PIC
}

void scheduler(){
    /*
    EXAMPLE:
    1) Counter is executing
    2) A PIT Interrupt occurs -> PIT handler executes -> Calls scheduling algorithm
    3) Context switch to next program in scheduling queue 
    4) Return from PIT handler and execute fish 
    5) Another PIT interrupt occurs!
    6) Context switch to pingpong
    7) Return from PIT Handler and execute pingpong
    8) Repeat...
    */
    process_control_block_t* schedule_pcb;

    /* schedule the next terminal to be run */
    if(set_next_scheduled() == -1) {
        return;
    }
    int32_t next_scheduled_idx = get_scheduled_term_idx();

    /* use index to recover context of next scheduled terminal's current PCB */
    /* maybe use the cur_term_pid field and do a calculation? not sure */

    int32_t schedule_pid = terminal_array[next_scheduled_idx].cur_term_pid;
    if (schedule_pid == 0){
        return;
    }
    int32_t schedule_addr = calculate_pcb_addr(schedule_pid);
    schedule_pcb = (process_control_block_t*) schedule_addr;

    tss.esp0 = schedule_pcb->tss_esp0;

    allocate_tasks(schedule_pid);
   
    schedule_switch(schedule_pcb->ebp);
    

    return;
}

/*
https://wiki.osdev.org/Programmable_Interval_Timer#Mode_0_.E2.80.93_Interrupt_On_Terminal_Count
*/
void PIT_handler(){
    //in the PIT handler’s assembly linkage, the 
    //registers restored will be the target process’s 
    //context information (including the EIP)
    
    //allow output to go low again
    outb(0x43, 0x1ADB0);
    //need ^ or below or both idk
    outb(0x40, 0x9C);		// Low byte
	outb(0x40, (0x2E00)>>8);	// High byte

    //send end of interrupt signal
    send_eoi(0);

    //if an interrupt occurs, call scheduler 
    scheduler();
}

int32_t PIT_frequency(int32_t freq){
    return 0;
}

int32_t PIT_open(const uint8_t* filename){
    return 0;
}

int32_t PIT_read(int32_t fd, void* buf, int32_t nbytes){
    return 0;
}

int32_t PIT_write(int32_t fd, const void* buf, int32_t nbytes){
    return 0;
}

int32_t PIT_close(int32_t fd){
    return 0;
}





