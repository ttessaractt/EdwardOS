/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */


/* i8259_init
 *  Functionality: initilizes the 8259 PIC for a master with a slave at IRQ2
 *  Arguments: none
 *  Return: none
code reference from OSdev wiki - 8259 PIC - Initialisation
https://wiki.osdev.org/8259_PIC
*/
void i8259_init(void) {
    outb(MASK_OUT_MASTER, MASTER_DATA); 
    outb(MASK_OUT_SLAVE, SLAVE_DATA);

    //Master INIT
    outb(ICW1, MASTER_8259_PORT);       // starts the initialization sequence
    outb(ICW2_MASTER, MASTER_DATA);     // ICW2: Master PIC vector offset
    outb(ICW3_MASTER, MASTER_DATA);     // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100) 
    outb(ICW4, MASTER_DATA);            // ICW4: have the PICs use 8086 mode (and not 8080 mode)

    //Slave INIT
    outb(ICW1, SLAVE_8259_PORT);        // starts the initialization sequence
    outb(ICW2_SLAVE, SLAVE_DATA);       // ICW2: Master PIC vector offset
    outb(ICW3_SLAVE, SLAVE_DATA);       // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(ICW4, SLAVE_DATA);             // ICW4: have the PICs use 8086 mode (and not 8080 mode)

    //restore saved masks
    outb(MASK_OUT_MASTER, MASTER_DATA);
    outb(MASK_OUT_SLAVE, SLAVE_DATA);

    enable_irq(0x02);       //enable the keyboard interrupts
}


/* enable_irq
 *  Functionality: enables the IRQ at the specified number
 *  Arguments: irq_num - the IRQ number to enable
 *  Return: none
code reference from OSdev wiki - 8259 PIC - Masking
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
    value = inb(port) & ~(1 << irq_num);
    //send value to IMR
    outb(value, port);
}

/* Disable (mask) the specified IRQ */
/* disable_irq
 *  Functionality: disables the IRQ at the specified number
 *  Arguments: irq_num - the IRQ number to disable
 *  Return: none
code reference from OSdev wiki - 8259 PIC - Masking
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

/* send_eoi
 *  Functionality: send the end of interrupt signal for the specified IRQ
 *  Arguments: irq_num - the IRQ number to send EOI for
 *  Return: none
code reference from OSdev wiki - 8259 PIC - End of Interrupt
https://wiki.osdev.org/8259_PIC
*/

void send_eoi(uint32_t irq_num) {
    if(irq_num >= 8){   //checks if IRQ is SLAVE
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT); //issue command to SLAVE
        outb(EOI | 2, MASTER_8259_PORT); //issue command to MASTER
        sti();
    }
    else{       //irq is just master
        outb(EOI | irq_num, MASTER_8259_PORT); //issue command to MASTER
        sti();
    }

}
