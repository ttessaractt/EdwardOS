#include "rtc.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"
#include "idt.h"

//flag for RTC_read()
flag interrupt_occured = false;

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
    //interrupt has occured
    interrupt_occured = true;
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

//virtualization of RTC frequency
//
/*
store frequency in a data structure that is accessed later
  
virtualization gives the illusion to the process that the RTC is set to their frequency whenin reality the RTC isn’t. 
Instead you set the RTC to the highest possible frequency and wait until you receive
x interrupts to return back to the process such that those x interrupts at the highest frequency is equivalent to 1
interrupt at the frequency the process wants
*/
/*
some code taken from OSdev wiki - RTC - Changing Interrupt Rate
https://wiki.osdev.org/RTC
input is the desired frequency in Hz 
*/
int32_t RTC_frequency(int32_t freq){
    int32_t rate;
    //disable other interrupts (SUPER IMPORTANT)
    cli(); //might need to be cli_and_save() ? 
    NMI_disable();  //disable NMI
    
    //freqency is between 2 and 1024 Hz
    if (freq < 2 && freq > 1024){
        //enable interrupts before returning
        NMI_enable();   //enable NMI
        sti(); 
        return -1;
    }
    //freqnecy must be a power of 2
    if ((freq & (freq - 1)) != 0){
        //enable interrupts before returning
        NMI_enable();   //enable NMI
        sti(); 
        return -1;
    }

    /*
    RS bits control freq, bits [3:0] of Register A
    RS3 RS2 RS1 RS0

    2Hz = 1111
    1024Hz = 0110
    */
    /*
    1111 = 15
    0010 = 2

    rate 3 = 8kHz
    rate 5 = 1024Hz
    rate 15 = 2Hz
    */
    //to get rate need to get what power of 2 the frequency is
    //go through bits in freq to find which is set to 1
    int32_t i = 0x10;   //start at 2
    int32_t pos = 1;
    //go until i matches freq
    while (!(i == freq)){
        if (pos > 11){      //highest power of 2 freqnecy can be set to
            //enable interrupts before returning
            NMI_enable();   //enable NMI
            sti(); 
            return -1; 
        }
        i = i << 1;         //move 1 to next but, ex. go from 0x10 to 0x100
        pos++;              //increment position
    }

    //lower rate give higher freqency so need to subtract position from 16 (15+1, max rate+1 to account for pos starting at 1)
    rate = 16 - pos;
    //printf("input rate = %d\n", rate);

    //freq =  32768 >> (rate-1);
    //printf("freq = %d\n", freq);
    //rate &= 0x0F;			// rate must be above 2 and not over 15
    //printf("set rate = %d\n", rate);
    //disable other interrupts (SUPER IMPORTANT)
    //cli(); //might need to be cli_and_save() ? but not sure
    //NMI_disable();  //disable NMI

    char prev = inb(RTC_PORT2);	// get initial value of register A
    //printf("prev = %c\n", prev);
    outb(REG_A, RTC_PORT1);		// reset index to A
    outb((prev & 0xF0) | rate, RTC_PORT2); //write only our rate to A. Note, rate is the bottom 4 bits.

    //enable interrupts
    NMI_enable();   //enable NMI
    sti(); 

    return 0;
};

//reset frequency to 2Hz
/*
successful call returns 0
uncessesful returns -1

The open system call provides access to the file system. The call should find the directory entry corresponding to the
named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1.
*/
int32_t RTC_open(const uint8_t* filename){
    //set frequency to 2Hz
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
    interrupt_occured = false;

    while (volatile interrupt_occured == false);    //wait until interrupt is called
    
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
