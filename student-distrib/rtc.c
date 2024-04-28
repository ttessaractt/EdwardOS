#include "rtc.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"
#include "idt.h"
#include "terminal.h"

//flag for RTC_read(), keeps track of if interrupt occured
int volatile interrupt_occured[3] = {0, 0, 0};
//array to store frequencies for 3 terminals, 0 means an open has not been called
int32_t terminal_freq[3] = {0, 0, 0};
//array of counters for rtc virtualization
int32_t terminal_count[3] = {0, 0, 0};
//flag to indicate if freqeuncy set is 1024
int32_t flag_1024[3] = {0, 0, 0};
/* RTC_init
 *  Functionality: enables interrupt on IRQ8 on PIC for RTC (real time clock) functionality
 *  Arguments: none
 *  Return: none
code taken from OSdev wiki - RTC - Turning on IRQ 8
https://wiki.osdev.org/RTC
*/
void RTC_init(){
    //enable RTC on PIC
    NMI_disable();
    outb(REG_B, RTC_PORT1);             //select register B and disable NMI (not necessary since interrupts are not enabled yet)
    char prev = inb(RTC_PORT2);         //read current value of register B
    outb(REG_B, RTC_PORT1);             //set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_PORT2);       //write the previous valued ORed with 0x40. This turns on bit 6 of register B
    
    //set init rate (512 Hz)
    outb(REG_A, RTC_PORT1);                         //set index to register A
    char prev1 = inb(RTC_PORT2);	                    //get initial value of register A
    outb(REG_A, RTC_PORT1);		                    //reset index to A
    outb(((prev1 & TOP4BITMASK) | 0x7), RTC_PORT2);

    outb(REG_C, RTC_PORT1);	    // select register C
    inb(RTC_PORT2);	

    enable_irq(8);                      //enable IRQ 8 
    NMI_enable();
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
    //increment freq counters for each terminal
    if (terminal_freq[1] != 0){
        terminal_count[1] += terminal_freq[1];
    }
    if (terminal_freq[2] != 0){
        terminal_count[2] += terminal_freq[2];
    }
    if (terminal_freq[0] != 0){
        terminal_count[0] += terminal_freq[0];
    }

    //do rtc virtualization for interrupt handling
    if ((terminal_freq[0] != 0) && (terminal_count[0] >= 512)){
        //printf_term("term: %d\n", terminal_count[0]);
        interrupt_occured[0] = 1;      //set interrupt_occured flag
        terminal_count[0] = 0;
    }
    if ((terminal_freq[1] != 0) && (terminal_count[1] >= 512)){
        //printf_term("term: %d\n", terminal_count[0]);
        interrupt_occured[1] = 1;      //set interrupt_occured flag
        terminal_count[1] = 0;
    }
    if ((terminal_freq[2] != 0) && (terminal_count[2] >= 512)){
        //printf_term("term: %d\n", terminal_count[0]);
        interrupt_occured[2] = 1;      //set interrupt_occured flag
        terminal_count[2] = 0;
    }
    // if ((terminal_freq[term] != 0) & (terminal_count[term] >= 1024)){
    //     printf_term("term: %d\n", terminal_count[term]);
    //     interrupt_occured[term] = 1;      //set interrupt_occured flag
    //     terminal_count[term] = 0;
    // }

    //allow another interrupt to happen
    outb(REG_C, RTC_PORT1);	    // select register C
    inb(RTC_PORT2);		        // just throw away contents

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

    //set rate for correct terminal
    int32_t term = get_scheduled_term_idx();
    terminal_freq[term] = freq;
    terminal_count[term] = 0;

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
    int32_t term = get_scheduled_term_idx();    //get scheduled index
    interrupt_occured[term] = 0;                //reset flag
    terminal_freq[term] = 2;                    //set freq to 2Hz
    terminal_count[term] = 0;                   //reset counter
    return 0;

};

/* RTC_read
 *  Functionality: return only after an RTC interrupt has occured for that terminal
 *  Arguments: fd - file device to read from (not used)
 *             buf - buffer of data (not used)
 *             nbytes - bytes to read (not used)
 *  Return: 0 - successful call
 */
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes){
    //reset count
    int32_t term = get_scheduled_term_idx();    //get scheduled index
    while (interrupt_occured[term] != 1);       //wait until interrupt is called
    interrupt_occured[term] = 0;                //reset flag
    return 0;                                   //return from function
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
    int32_t term = get_scheduled_term_idx();    //get scheduled index
    interrupt_occured[term] = 0;                //reset flag
    terminal_freq[term] = 0;                    //set freq to 2Hz
    terminal_count[term] = 0;                   //reset counter
    return 0;
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

