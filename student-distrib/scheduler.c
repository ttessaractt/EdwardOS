#include "types.h"
#include "i8259.h"
#include "rtc.h"
#include "lib.h"
#include "terminal.h"
#include "file.h"
#include "x86_desc.h"
#include "schedule_switch.h"
#include "paging.h"
#include "syscalls.h"

int volatile pit_interrupt_occured = 0;
int active_processes[3] = {0, 0, 0}; 
terminal_t terminal_array[3];
extern int32_t boot_flag;
extern process_control_block_t* current_process;
extern int32_t current_pid;            // initial pid = 0
extern int32_t current_parent_pid;

/*
https://wiki.osdev.org/Programmable_Interval_Timer#Mode_0_.E2.80.93_Interrupt_On_Terminal_Count
*/

/* void PIT_init();
 * Description: initializes the programmable interval timer (pit)   
 * Inputs: none
 * Return Value: none
 */
void PIT_init(){
    //0x43 - Mode/Command register (write only, a read is ignored)
    //00110000 = 0x30
    //set PIT mode/command register
    //printf("init\n");
    //cli();
    //outb(0x43, 0x30);     //theoretically set pit mode/command register
    
    //set low&high byte of reload value
    //time in ms = reload_value * 3000 / 3579545
    //10ms = 0x9C2E, 0x9C & (0x2E00)>>8
	//outb(0x40, 0x01);		// Low byte
	//outb(0x40, (0x0000)>>8);	// High byte
    //cli();
    int divisor = 1193180 / 100;       /* Calculate our divisor */
    outb(0x36, 0x43);             /* Set our command byte 0x36 */
    outb(divisor & 0xFF, 0x40);   /* Set low byte of divisor */
    outb(divisor >> 8, 0x40);     /* Set high byte of divisor */
    //sti();
    //enable IRQ0 on PIC
    enable_irq(0);          //enable PIT interrupt on PIC
    //sti();

    // currently 20 hz?
}

/* void scheduler();
 * Description: handles scheduling by saving the context of the previous program
                and restoring the scheduled program that becomes the actively running
                program. gets called in PIT_handler
 * Inputs: none
 * Return Value: none
 */
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
   cli();
    process_control_block_t* schedule_pcb;
    process_control_block_t* schedule_pcb_old;

    if ((boot_flag != 0) & (boot_flag != -1)){
        register uint32_t saved_ebp asm("ebp");     //save ebp
        int32_t old_scheduled_idx = get_scheduled_term_idx();
        int32_t old_schedule_pid = terminal_array[old_scheduled_idx].cur_term_pid;
        int32_t old_schedule_addr = calculate_pcb_addr(old_schedule_pid);
        schedule_pcb_old = (process_control_block_t*) old_schedule_addr;
        schedule_pcb_old->ebp_switch = saved_ebp;
    }


    /* schedule the next terminal to be run */
    if(set_next_scheduled() == -1) {
        return;
    }
    int32_t next_scheduled_idx = get_scheduled_term_idx();

    if(terminal_array[next_scheduled_idx].shell_exists == 0){
        if ((terminal_array[next_scheduled_idx].active && terminal_array[next_scheduled_idx].scheduled)){
            boot_flag = 1;
        }

        terminal_array[next_scheduled_idx].shell_exists = 1;

        sti();
        execute((uint8_t*)"shell");

        return;
    }
   

    if ((terminal_array[next_scheduled_idx].active && terminal_array[next_scheduled_idx].scheduled)){
            page_table_vid_mem[0].pf_addr = 0xB8000 >> 12;
       }
    else{
           page_table_vid_mem[0].pf_addr = (OFFSET_1MB + (next_scheduled_idx) * OFFSET_4KB) >> 12;
    }
    
    flush_tlb();

    int32_t schedule_pid = terminal_array[next_scheduled_idx].cur_term_pid;
    if (schedule_pid == 0){
        sti();
        return;
    }
    int32_t schedule_addr = calculate_pcb_addr(schedule_pid);
    schedule_pcb = (process_control_block_t*) schedule_addr;

    tss.esp0 = schedule_pcb->tss_esp0;

    current_process = schedule_pcb;
    current_pid = schedule_pid;
    current_parent_pid = schedule_pcb->parent_pid;

    allocate_tasks(schedule_pid);
    
    sti();
    schedule_switch(schedule_pcb->ebp_switch);
    

    return;
}

/*
https://wiki.osdev.org/Programmable_Interval_Timer#Mode_0_.E2.80.93_Interrupt_On_Terminal_Count
*/

/* void PIT_handler();
 * Description: is called when a pit interrupt occurs; basically a gateway
                to the scheduler function
 * Inputs: none
 * Return Value: none
 */
void PIT_handler(){

    send_eoi(0);

    //if an interrupt occurs, call scheduler 
    scheduler();
}





