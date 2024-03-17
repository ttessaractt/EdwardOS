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
    a1 = inb(MASTER_DATA);
    a2 = inb(SLAVE_DATA);

    // starts the initialization sequence (in cascade mode)
    outb(MASTER_8259_PORT, ICW1 | ICW4);
    outb(SLAVE_8259_PORT, ICW1 | ICW4);

    // ICW2: Master PIC vector offset
    outb(MASTER_DATA, ICW2_MASTER);
    outb(SLAVE_DATA, ICW2_SLAVE);

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) 
    outb(MASTER_DATA, 4);
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(SLAVE_DATA, 2);

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    outb(MASTER_DATA, 0x01);
    outb(SLAVE_DATA, 0x01);

    //restore saved masks
    outb(MASTER_DATA, a1);
    outb(SLAVE_DATA, a2);
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
    //set bit at irq_num to 1
    value = inb(port) | (1 << irq_num);
    //send value to IMR
    outb(port, value);
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
    //set bit at irq_num to 0
    value = inb(port) & ~(1 << irq_num);
    //send value to IMR
    outb(port, value);
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
        outb(MASTER_8259_PORT, EOI); //issue command to SLAVE
    }
    //command is always send to MASTER
    outb(SLAVE_8259_PORT, EOI); //issue command to MASTER
}
