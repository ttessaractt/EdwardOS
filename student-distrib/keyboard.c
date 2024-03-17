#include "keyboard.h"
#include "i8259.h"
#include "types.h"
#include "lib.h"

//initilize keyboard
//enable associated inerrupt on PIC
void keyboard_init(){


}

//keyboard_handler
void keyboard_handler(){
    uint32_t key = inb(0x60);
    //int8_t* buf;

    //itoa(key, buf, hex);

    printf("key: %x\n", key);
    send_eoi(KEYBOARD_VECTOR);
};
