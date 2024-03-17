/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/* code taken from OSdev wiki - 8259 PIC - Initialisation
https://wiki.osdev.org/8259_PIC
*/
void i8259_init(void) {
    uint8_t a1, a2;

    //save masks
    //a1 = inb(MASTER_DATA);
    //a2 = inb(SLAVE_DATA);

    outb(0xFB, MASTER_DATA);
    outb(0xFF, SLAVE_DATA);

    // starts the initialization sequence (in cascade mode)
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_DATA);
    outb(ICW3_MASTER, MASTER_DATA);
    outb(ICW4, MASTER_DATA);


    outb(ICW1, SLAVE_8259_PORT);

    // ICW2: Master PIC vector offset
    
    outb(ICW2_SLAVE, SLAVE_DATA);

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) 
    
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(ICW3_SLAVE, SLAVE_DATA);

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    
    outb(ICW4, SLAVE_DATA);

    //restore saved masks
    outb(0xFB, MASTER_DATA);
    outb(0xFF, SLAVE_DATA);

    enable_irq(2);
}


/* Enable (unmask) the specified IRQ */
/* Initialize the 8259 PIC */
/* code taken from OSdev wiki - 8259 PIC - Masking
https://wiki.osdev.org/8259_PIC
*/
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;

    //get correct port based on MASTER vs SLAVE
    if (irq_num < 8){ //IRQ is MASTER 
        port = MASTER_DATA;
    }
    else {  //IRQ is SLAVE
        port = SLAVE_DATA;
        irq_num -= 8;
    }
    //set bit at irq_num to 0
    value = inb(port) & ~(1 << irq_num); // flip with disable?
    //send value to IMR
    outb(value, port);
}

/* Disable (mask) the specified IRQ */
/* Initialize the 8259 PIC */
/* code taken from OSdev wiki - 8259 PIC - Masking
https://wiki.osdev.org/8259_PIC
*/
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;

    //get correct port based on MASTER vs SLAVE
    if (irq_num < 8){ //IRQ is MASTER 
        port = MASTER_DATA;
    }
    else {  //IRQ is SLAVE
        port = SLAVE_DATA;
        irq_num -= 8;
    }
    //set bit at irq_num to 1
    value = inb(port) | (1 << irq_num);
    //send value to IMR
    outb(value, port);
}

/* Send end-of-interrupt signal for the specified IRQ */
/* code taken from OSdev wiki - 8259 PIC - End of Interrupt
https://wiki.osdev.org/8259_PIC
*/
/*
notes:
if IRQ came from master PIC only need to issue to master PIC, but if it came
from slave PIC it needs to be sent to both PIC chips
if irq < 8 it is master PIC
if irq >= 8 it is a slave PIC
*/
void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8){   //checks if IRQ is SLAVE
        outb(EOI | irq_num, SLAVE_8259_PORT); //issue command to SLAVE
    }
    //command is always send to MASTER
    outb(EOI | irq_num, MASTER_8259_PORT); //issue command to MASTER
    sti();
}
