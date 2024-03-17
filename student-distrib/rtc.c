#include "rtc.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"
#include "idt.h"

//initilize RTC
//enable associated inerrupt on PIC
/*
code taken from OSdev wiki - RTC - Turning on IRQ 8
https://wiki.osdev.org/RTC
*/
void RTC_init(){
    //enable RTC on PIC
    enable_irq(0x08);
    
};

/*
code taken from OSdev wiki - RTC - Interrupts and Register C
https://wiki.osdev.org/RTC
*/
//RTC handler
void RTC_handler(){
    //disable other interrupts (SUPER IMPORTANT)
    cli(); //might need to be cli_and_save() ? but not sure
    NMI_disable();  //disable NMI

    //handler stuff
    printf("RTC interrupt\n");
    test_interrupts();

    //make sure we get another interrupt
    outb(REG_C, RTC_PORT1);	// select register C
    inb(RTC_PORT2);		// just throw away contents
    //enable other interrupts
    NMI_enable();   //enable NMI
    sti(); 
};

/*
NMI_enable and NMI_disable taken from OSdev wiki - NMI - USage
https://wiki.osdev.org/NMI
*/
 void NMI_enable() {
    outb(inb(RTC_PORT1) & 0x7F, RTC_PORT1);   //unmask NMI
    inb(RTC_PORT2);  //prevent CMOS RTC from going into undefined state
 }
 
 void NMI_disable() {
    outb(inb(RTC_PORT1) | 0x80, RTC_PORT1);   //mask NMI
    inb(RTC_PORT2);  //prevent CMOS RTC from going into undefined state
 }
