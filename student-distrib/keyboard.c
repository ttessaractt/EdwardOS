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

    //printf("key: %x\n", key);

    const char key_num[10] = {
        '1', '2', '3', '4', '5',    
        '6', '7', '8', '9', '0'     
    };

    const char key_QP[10] = {
        'q', 'w', 'e', 'r', 't',    
        'y', 'u', 'i', 'o', 'p'     
    };

    const char key_AL[9] = {
        'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l'     
    };

    const char key_ZM[7] = {
        'z', 'x', 'c', 'v', 'b',    
        'n', 'm'     
    };

    char p;

    if ((2 <= key) && (key<= 11)){
        p = key_num[key-2];
        printf("%c", p);
    }
    else if ((16 <= key) && (key <= 25)){
        p = key_QP[key-16];
        printf("%c", p);
    }
    else if ((30 <= key) && (key <= 38)){
        p = key_AL[key-30];
        printf("%c", p);
    }
    else if ((44 <= key) && (key <= 50)){
        p = key_ZM[key-44];
        printf("%c", p);
    }
    else{
        printf("hi");
    }


    send_eoi(KEYBOARD_VECTOR);
};
