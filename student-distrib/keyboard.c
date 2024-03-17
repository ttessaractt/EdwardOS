// keyboard file

#include "keyboard.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"


// to get keyboard interrupts, enable_irq(1)

void keyboard_init(){

    enable_irq(1); // enables interrupt on IRQ1 on PIC

    outb(KEYBOARD_DATA, 0xF0);
    outb(KEYBOARD_DATA, 0x01); // keyboard in scan code mode 1

    outb(KEYBOARD_DATA, 0xF4); // enable scanning 

    return;


}

void keyboard_handler(){
    uint32_t key = inb(0x60);
    //int8_t* buf;

    //itoa(key, buf, hex);

    printf("key: %x\n", key);
    send_eoi(KEYBOARD_VECTOR);
};
