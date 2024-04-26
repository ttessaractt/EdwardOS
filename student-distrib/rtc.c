#include "rtc.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"
#include "idt.h"

//flag for RTC_read(), keeps track of if interrupt occured
int volatile interrupt_occured1 = 0;
int volatile interrupt_occured2 = 0;
int volatile interrupt_occured3 = 0;

int32_t terminal1_freq = 0;
int32_t terminal2_freq = 0;
int32_t terminal3_freq = 0;
int32_t terminal1_count = 0;
int32_t terminal2_count = 0;
int32_t terminal3_count = 0;
/* RTC_init
 *  Functionality: enables interrupt on IRQ8 on PIC for RTC (real time clock) functionality
 *  Arguments: none
 *  Return: none
code taken from OSdev wiki - RTC - Turning on IRQ 8
https://wiki.osdev.org/RTC
*/
void RTC_init(){
    //enable RTC on PIC
    outb(REG_B, RTC_PORT1);             //select register B and disable NMI (not necessary since interrupts are not enabled yet)
    char prev = inb(RTC_PORT2);         //read current value of register B
    outb(REG_B, RTC_PORT1);             //set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_PORT2);       //write the previous valued ORed with 0x40. This turns on bit 6 of register B
    enable_irq(8);                      //enable IRQ 8 
    //set init rate
    outb(REG_A, RTC_PORT1);                         //set index to register A
    char prev1 = inb(RTC_PORT2);	                    //get initial value of register A
    outb(REG_A, RTC_PORT1);		                    //reset index to A
    outb((prev1 & TOP4BITMASK) | 0x03, RTC_PORT2);
};

/* RTC_handler
 *  Functionality: when an RTC interrupt occurs set the interrupt_occured flag and print 1 to the terminal
 *  Arguments: none
 *  Return: none
some code taken from OSdev wiki - RTC - Interrupts and Register C
https://wiki.osdev.org/RTC*/
void RTC_handler(){
    //disable other interrupts (SUPER IMPORTANT)
    cli();                      //disable interrupts
    NMI_disable();              //disable NMI
    //allow another interrupt to happen

    terminal1_count += terminal1_freq;
    terminal2_count++;
    terminal3_count++;


    //rtc is in 1024 Hz
    //conver to whatever freq is
    if (terminal1_count == MAX_FREQ){
        interrupt_occured1 = 1;      //set interrupt_occured flag
        terminal1_count = 0;
    }



    outb(REG_C, RTC_PORT1);	    // select register C
    inb(RTC_PORT2);		        // just throw away contents
    //interrupt handling
    //interrupt_occured1 = 1;      //set interrupt_occured flag
    //printf("1");                //show the interrupt occured in terminal
    send_eoi(8);                //signal end of interrupt to PIC
    //enable interrupts
    NMI_enable();               //enable NMI
    sti();                      //enable interrupts

};


/* RTC_frequency
 *  Functionality: checks if the input frequency is valid and if it is, calcuates and sets the rate needed
 *                  for the RTC to have that frequency
 *  Arguments: freq - the desired frequency to set the RTC to in Hz
 *  Return: 0 - frequency was successfully set
 *          -1 - the input frequency is not a valid frequency to set the RTC to
some code taken from OSdev wiki - RTC - Changing Interrupt Rate
https://wiki.osdev.org/RTC
*/
int32_t RTC_frequency(int32_t freq){
    int32_t rate = 0;       //rate is how the frequency is set in RTC register A
    int32_t i = ONEHEX;        //number to match to freq, start at 0x01, for finding rate
    int32_t pos = 0;        //position of 1 bit in freq, for finding rate
    int32_t flags;          //store flags
    //disable other interrupts (SUPER IMPORTANT)
    cli_and_save(flags);    //diable interrupts & save flags
    NMI_disable();          //disable NMI
    //check if frequency is between 2 and 1024 Hz
    if (freq < MIN_FREQ && freq > MAX_FREQ){
        //enable interrupts before returning
        restore_flags(flags);               //restore flags
        NMI_enable();                       //enable NMI
        sti();                              //enable interrupts
        return -1;                          //freq not in bounds, return -1
    }
    //check if freqnecy is a power of 2
    if ((freq & (freq - 1)) != 0){
        //enable interrupts before returning
        restore_flags(flags);               //restore flags
        NMI_enable();                       //enable NMI
        sti();                              //enable interrupts
        return -1;                          //freq not power of 2, return -1
    }
    //to get rate need to get what power of 2 the frequency is
    //go through bits in freq to find which is set to 1
    /*while (!(i == freq)){                   //go until i matches freq
        if (pos > MAX_RATE){                //highest valid power of 2 freqnecy can be set to
            //enable interrupts before returning
            restore_flags(flags);           //restore flags
            NMI_enable();                   //enable NMI
            sti();                          //enable interrupts
            return -1;                      //freq not in bounds, return -1
        }
        i = i << 1;                         //move 1 to next bit, ex. go from 0x10 to 0x100
        pos++;                              //increment position
    }*/
    //lower rate give higher frequency so need to subtract position from 16 (15+1, max rate+1 to account for pos starting at 1)
    
    rate = POSRATE_OFFSET - pos;                    //get rate
    terminal1_freq = freq;
    terminal2_freq = freq;
    terminal3_freq = freq;
    terminal1_count = 0;

    //set rate in register A
    //outb(REG_A, RTC_PORT1);                         //set index to register A
    //char prev = inb(RTC_PORT2);	                    //get initial value of register A
    //outb(REG_A, RTC_PORT1);		                    //reset index to A
    //outb((prev & TOP4BITMASK) | rate, RTC_PORT2);  //write only our rate to A. Note, rate is the bottom 4 bits.
    //enable interrupts
    restore_flags(flags);                           //restore flags
    NMI_enable();                                   //enable NMI
    sti();                                          //enable interrupts
    return 0;                                       //freq set successfully, return 0
};


/* RTC_open
 *  Functionality: resets the interrupt_occured flag & sets the freqency to 2Hz
 *  Arguments: filename - the filename (not used)
 *  Return: 0 - frequency was successfully set
 *          -1 - the input frequency is not a valid frequency to set the RTC to
 */
int32_t RTC_open(const uint8_t* filename){
    interrupt_occured1 = 0;                      //reset interrupt occured flag
    return RTC_frequency((int32_t)MIN_FREQ);    //set freqency to 2Hz
};

/* RTC_read
 *  Functionality: return only after an RTC interrupt has occured
 *  Arguments: fd - file device to read from (not used)
 *             buf - buffer of data (not used)
 *             nbytes - bytes to read (not used)
 *  Return: 0 - successful call
 */
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes){
    interrupt_occured1 = 0;              //reset interrupt_occured flag
    while (interrupt_occured1 != 1);     //wait until interrupt is called
    return 0;                           //return from function
};

/* RTC_read
 *  Functionality: reads new rate from buffer, then set the frequency for the RTC
 *  Arguments: fd - file device to read from (not used)
 *             buf - buffer with 4 byte integer specifiying frequency is Hz
 *             nbytes - bytes to read (not used)
 *  Return: 0 - successful call
 *          -1 - no frequency is sent or frequency is not valid
 */
int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes){
    if (buf == NULL){return -1;}            //check there is something in buf
    if (nbytes > 4){return -1;}             //check size of buf
    //get frequency from buffer
    const int* p = buf;                     
    int32_t buffer = p[0];                  
    return RTC_frequency(buffer);           //set freqency
};

/* RTC_close
 *  Functionality: resets the freqency to 2Hz
 *  Arguments: filename - the filename (not used)
 *  Return: 0 - frequency was successfully set
 *          -1 - the input frequency is not a valid frequency to set the RTC to
 */
int32_t RTC_close(int32_t fd){
    return RTC_frequency((int32_t)MIN_FREQ);    //set freqency to 2Hz
};

/*
NMI_enable and NMI_disable taken from OSdev wiki - NMI - USage
https://wiki.osdev.org/NMI
Enable and Disable Non Maskable Interrupts, used for modifying registers for RTC
*/
 void NMI_enable() {
    outb(inb(RTC_PORT1) & 0x7F, RTC_PORT1);   //unmask NMI
    inb(RTC_PORT2);  //prevent CMOS RTC from going into undefined state
 }
 void NMI_disable() {
    outb(inb(RTC_PORT1) | 0x80, RTC_PORT1);   //mask NMI
    inb(RTC_PORT2);  //prevent CMOS RTC from going into undefined state
 }
