#include "rtc.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"
#include "idt.h"

//flag for RTC_read()
int volatile interrupt_occured = 0;
int volatile freqency = 2;
int volatile rtc_count = 0;
//int volatile diff = MAX_FREQ/freqency;


/* RTC_init
 *  Functionality: enables interrupt on IRQ8 on PIC for RTC (real time clock) functionality
 *  Arguments: none
 *  Return: none
code taken from OSdev wiki - RTC - Turning on IRQ 8
https://wiki.osdev.org/RTC
*/
void RTC_init(){
    //enable RTC on PIC
    //cli();          //disable interrupts
    //NMI_disable();  //disable NMI
    outb(REG_B, RTC_PORT1);
    char prev = inb(RTC_PORT2);     //read current value of register B
    outb(0x8B, 0x70);               //set the index again
    outb(prev | 0x40, 0x71);          //turn on bit 6
    outb(REG_C, RTC_PORT1);	// select register C
    inb(RTC_PORT2);		// just throw away contents
    enable_irq(8);               //enable irq 8

    RTC_frequency(1024);
    //NMI_enable();   //enable NMI
    //sti(); 
};

/* RTC_handler
 *  Functionality: when an RTC interrupt occurs set the interrupt_occured flag and print 1 to the terminal
 *  Arguments: none
 *  Return: none
code taken from OSdev wiki - RTC - Interrupts and Register C
https://wiki.osdev.org/RTC
*/
//RTC handler
void RTC_handler(){
    //int32_t flags;
    //disable other interrupts (SUPER IMPORTANT)
    //cli_and_save(flags); //might need to be cli_and_save() ? but not sure
    cli();
    NMI_disable();  //disable NMI

    outb(REG_C, RTC_PORT1);	// select register C
    inb(RTC_PORT2);		// just throw away contents

    //rtc_count++;    //increment rtc_count at every interrupt
    //interrupt has occured
    interrupt_occured = 1;
    //handler stuff

    //printf("RTC interrupt\n");
    //test_interrupts();
    //outb(REG_B, RTC_PORT1);     //set index to register A
    //char prev = inb(RTC_PORT2);	// get initial value of register A
    //printf("reg B: %d\n", prev);

    printf("1");
    
    //make sure we get another interrupt
    //outb(REG_C, RTC_PORT1);	// select register C
    //inb(RTC_PORT2);		// just throw away contents
    //enable other interrupts

    send_eoi(8);

    NMI_enable();   //enable NMI
    sti(); 

};


/* RTC_freqency
 *  Functionality: checks if the input freqency is valid and if it is, calcuates and sets the rate needed
 *                  for the RTC to have that freqency
 *  Arguments: freq - the desired frequency to set the RTC to
 *  Return: none
some code taken from OSdev wiki - RTC - Changing Interrupt Rate
https://wiki.osdev.org/RTC
input is the desired frequency in Hz 
*/
int32_t RTC_frequency(int32_t freq){
    int32_t rate = 0;
    int32_t i = 0x01;   //start at 1
    int32_t pos = 0;
    int32_t next, prev;
    int32_t flags;
    int32_t dif = 0;
    //disable other interrupts (SUPER IMPORTANT)
    cli_and_save(flags); //might need to be cli_and_save() ? 
    NMI_disable();  //disable NMI

    //printf("freq1: %d\n", freq);
    //freqency is between 2 and 1024 Hz
    if (freq < 2 && freq > 1024){
        //enable interrupts before returning
        restore_flags(flags);
        NMI_enable();   //enable NMI
        sti(); 
        return -1;
    }
    //freqnecy must be a power of 2
    if ((freq & (freq - 1)) != 0){
        //enable interrupts before returning
        restore_flags(flags);
        NMI_enable();   //enable NMI
        sti(); 
        return -1;
    }
    //freqency = freq;
    //printf("freq2: %d\n", freq);
    //to get rate need to get what power of 2 the frequency is
    //go through bits in freq to find which is set to 1
    
    //go until i matches freq
    while (!(i == freq)){
        if (pos > 11){      //highest power of 2 freqnecy can be set to
            //enable interrupts before returning
            restore_flags(flags);
            NMI_enable();   //enable NMI
            sti(); 
            return -1; 
        }
        //printf("pos: %d\n", pos);
        i = i << 1;         //move 1 to next but, ex. go from 0x10 to 0x100
        pos++;              //increment position
    }
    //printf("freq3: %d\n", freq);
    //lower rate give higher freqency so need to subtract position from 16 (15+1, max rate+1 to account for pos starting at 1)
    rate = 16 - pos;    //calculated correctly
    dif = 10 - rate;
    //diff = dif;
    //printf("pos: %d\n", pos);
    //printf("rate: %d\n", rate);

    //test_interrupts();


    outb(REG_A, RTC_PORT1);     //set index to register A
    prev = inb(RTC_PORT2);	// get initial value of register A
    //printf("prev: %d\n", prev);
    outb(REG_A, RTC_PORT1);		// reset index to A
    next = (prev & 0xF0) | rate;
    //printf("next: %d\n", next);
    outb(next, RTC_PORT2); //write only our rate to A. Note, rate is the bottom 4 bits.

    prev = inb(RTC_PORT2);
    //printf("new: %d\n", prev);

    //enable interrupts
    restore_flags(flags);
    NMI_enable();   //enable NMI
    sti(); 

    return 0;
};


/* RTC_open
 *  Functionality: enables interrupt on IRQ1 on PIC for keyboard functionality
 *  Arguments: none
 *  Return: none
 */
int32_t RTC_open(const uint8_t* filename){
    //set frequency to 2Hz
    interrupt_occured = 0;
    return RTC_frequency((int32_t)2);      //hypothetically (needs testing) set freq to 2Hz

    //return 0;
};

//only returns after an interrupt has been called, use flag
/*
successful call returns 0
uncessesful returns -1

For the real-time clock (RTC), this call should always return 0, but only after an interrupt has
occurred (set a flag and wait until the interrupt handler clears it, then return 0). 
*/
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes){
    interrupt_occured = 0;
    //outb(REG_B, RTC_PORT1);     //set index to register A
    //char prev = inb(RTC_PORT2);	// get initial value of register A
    //printf("reg B: %d\n", prev);
    //printf("waiting read\n");
    while (interrupt_occured != 1){
        //printf("2");
    };    //wait until interrupt is called
    //interrupt_occured = 1;
    //printf("done read\n");
    return 0;
};

//get input parameters through a buffer, not read the value directly
/*
successful call returns 0
uncessesful returns -1

In the case of the RTC, the system call should always accept only a 4-byte
integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly
*/
int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes){
    if (buf == NULL){       //nothing in buf
        return -1;
    }

    int32_t freq = (int32_t)buf;    //get interrupt rate from buffer

    printf("write freq: %d\n", freq);
    //set freqency
    return RTC_frequency(freq);

    //return 0;
};

/*
successful call returns 0
uncessesful returns -1

The close system call closes the specified file descriptor and makes it available for return from later calls to open.
You should not allow the user to close the default descriptors (0 for input and 1 for output). Trying to close an invalid
descriptor should result in a return value of -1; successful closes should return 0.
*/
int32_t RTC_close(int32_t fd){
    return 0;
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
