#include "rtc.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"

//initilize RTC
//enable associated inerrupt on PIC
/*
code taken from OSdev wiki - RTC - Turning on IRQ 8
https://wiki.osdev.org/RTC
*/
void RTC_init(){
    //disable other interrupts (SUPER IMPORTANT)
    cli(); //might need to be cli_and_save() ? but not sure
    NMI_disable();  //disable NMI
    outb(RTC_PORT1, REG_B);		// select register B, and disable NMI
    //turn on periodic interrupt in RTC
    char prev = inb(RTC_PORT2);	// read the current value of register B
    outb(RTC_PORT1, REG_B);		// set the index again (a read will reset the index to register D)
    outb(RTC_PORT2, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    //enable RTC on PIC
    enable_irq(2);
    //enable other interrupts
    sti(); 
    NMI_enable();   //enable NMI

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
    outb(RTC_PORT1, REG_C);	// select register C
    inb(RTC_PORT2);		// just throw away contents
    //enable other interrupts
    sti(); 
    NMI_enable();   //enable NMI
};


/*
NMI_enable and NMI_disable taken from OSdev wiki - NMI - USage
https://wiki.osdev.org/NMI
*/
 void NMI_enable() {
    outb(RTC_PORT1, inb(RTC_PORT1) & 0x7F);   //unmask NMI
    inb(RTC_PORT2);  //prevent CMOS RTC from going into undefined state
 }
 
 void NMI_disable() {
    outb(RTC_PORT1, inb(RTC_PORT1) | 0x80);   //mask NMI
    inb(RTC_PORT2);  //prevent CMOS RTC from going into undefined state
 }
