//file with IDT

#include "lib.h"        //printf support
#include "x86_desc.h"   //for IDT entry
//inerupt support
//syscall support
#include "idt.h"
#include "keyboard.h"
//#include "rtc.h"
#include "syscalls.h"


/*
 An interrupt descriptor entry (goes into the IDT) 
typedef union idt_desc_t {
    uint32_t val[2];
    struct {
        uint16_t offset_15_00;
        uint16_t seg_selector;
        uint8_t  reserved4;
        uint32_t reserved3 : 1;
        uint32_t reserved2 : 1;
        uint32_t reserved1 : 1;
        uint32_t size      : 1;
        uint32_t reserved0 : 1;
        uint32_t dpl       : 2;
        uint32_t present   : 1;
        uint16_t offset_31_16;
    } __attribute__ ((packed));
} idt_desc_t;

//reserved + size = gate type
interrupt:
    #  31                                16  15  14 13   12      8  7     5 4   0
    # |-------------------------------------------------------------------------|
    # |                                    |    |  D   | Gate Type |       |    |
    # | Offset 31:16                       | P  |  P   | 0 D 1 1 0 | 0 0 0 |    | 4
    # |                                    |    |  L   |           |       |    |
    # |-------------------------------------------------------------------------|
    # 31                                 16 15                                 0
    # |-------------------------------------------------------------------------|
    # |                                    |                                    |
    # | Segment Selector                   | Offset 15:0                        | 0
    # |                                    |                                    |
    # |-------------------------------------------------------------------------|

trap:
    #  31                                16  15  14 13   12      8  7     5 4   0
    # |-------------------------------------------------------------------------|
    # |                                    |    |  D   | Gate Type |       |    |
    # | Offset 31:16                       | P  |  P   | 0 D 1 1 1 | 0 0 0 |    | 4
    # |                                    |    |  L   |           |       |    |
    # |-------------------------------------------------------------------------|
    # 31                                 16 15                                 0
    # |-------------------------------------------------------------------------|
    # |                                    |                                    |
    # | Segment Selector                   | Offset 15:0                        | 0
    # |                                    |                                    |
    # |-------------------------------------------------------------------------|


 The IDT itself (declared in x86_desc.S 
idt_desc_t idt[NUM_VEC];
 The descriptor used to load the IDTR 
x86_desc_t idt_desc_ptr;

 Sets runtime parameters for an IDT entry 
#define SET_IDT_ENTRY(str, handler)                              \
do {                                                             \
    str.offset_31_16 = ((uint32_t)(handler) & 0xFFFF0000) >> 16; \
    str.offset_15_00 = ((uint32_t)(handler) & 0xFFFF);           \
} while (0)

*/

/*
 * set_trap_gate
 *  Description: set/change a trap gate to IDT at the specified interupt #
 *  Input:  num -- IDT number
 *          addr -- address of the handler to call
 *  Output: none
 *  Return Value: none
 *  Side Effects: specified IDT entry is set
*/
void set_trap_gate(uint32_t num, uint32_t addr){
    //They contain a far pointer (segment selector and offset) that the processor uses to transfer program
    //execution to a handler procedure in an exception- or interrupt-handler code segment. 

    //reserved + size = gate type
    //  0111   + 1(000) = 1111 (32-bit trap gate)
    //reserved[3:0] = 0111

    //idt[num].offset_15_00 = ;     //set by SET_IDT_ENTRY
    idt[num].seg_selector = KERNEL_CS;
    idt[num].reserved4 = EIGHTBIT;
    idt[num].reserved3 = ONE;
    idt[num].reserved2 = ONE;
    idt[num].reserved1 = ONE;
    idt[num].size = ONE;            //1 = 32 bits
    idt[num].reserved0 = ZERO;
    idt[num].dpl = DPL_ZERO;            //privilege level 0
    idt[num].present = ONE;         //set to 1 so descriptor is valid
    //idt[num].offset_31_16 = ;     //set by SET_IDT_ENTRY
    SET_IDT_ENTRY(idt[num], addr);
};

/*
 * set_interrupt_gate
 *  Description: set/change a interrupt gate to IDT at the specified interupt #
 *  Input:  num -- IDT number
 *          addr -- address of the handler to call
 *  Output: none
 *  Return Value: none
 *  Side Effects: specified IDT entry is set
*/
void set_interrupt_gate(uint32_t num, uint32_t addr){
    //They contain a far pointer (segment selector and offset) that the processor uses to transfer program
    //execution to a handler procedure in an exception- or interrupt-handler code segment. 

    //reserved + size = gate type
    //  0110   + 1(000) = 1110 (32-bit interrupt gate)
    //reserved[3:0] = 0110

    //idt[num].offset_15_00 = ;     //set by SET_IDT_ENTRY
    idt[num].seg_selector = KERNEL_CS;
    idt[num].reserved4 = EIGHTBIT;
    idt[num].reserved3 = ZERO;
    idt[num].reserved2 = ONE;
    idt[num].reserved1 = ONE;
    idt[num].size = ONE;            //1 = 32 bits
    idt[num].reserved0 = ZERO;
    idt[num].dpl = DPL_ZERO;            //privilege level 0
    idt[num].present = ONE;         //set to 1 so descriptor is valid
    //idt[num].offset_31_16 = ;     //set by SET_IDT_ENTRY
    SET_IDT_ENTRY(idt[num], addr);
};  

/*
 * set_syscall_gate
 *  Description: set/change a syscall gate to IDT at the specified interupt #
 *  Input:  num -- IDT number
 *          addr -- address of the handler to call
 *  Output: none
 *  Return Value: none
 *  Side Effects: specified IDT entry is set
*/
void set_syscall_gate(uint32_t num, uint32_t addr){
    //idt[num].offset_15_00 = ;     //set by SET_IDT_ENTRY
    idt[num].seg_selector = KERNEL_CS;
    idt[num].reserved4 = EIGHTBIT;
    idt[num].reserved3 = ONE;
    idt[num].reserved2 = ONE;
    idt[num].reserved1 = ONE;
    idt[num].size = ONE;            //1 = 32 bits
    idt[num].reserved0 = ZERO;
    idt[num].dpl = DPL_THREE;            //privilege level 3, callable by user space
    idt[num].present = ONE;         //set to 1 so descriptor is valid
    //idt[num].offset_31_16 = ;     //set by SET_IDT_ENTRY
    SET_IDT_ENTRY(idt[num], addr);
};    

//will need to add set_systemcall_gate(uint32_t num, uint32_t addr){} with DPL=3
//^^ this is for something specific but i did not write down

/*
 * idt_init
 *  Description: initilize IDT table
 *  Input: none
 *  Output: none
 *  Return Value: none
 *  Side Effects: IDT table is set
*/
void idt_init(){

    set_trap_gate(0, (uint32_t)&divide_error);
    set_trap_gate(1, (uint32_t)&debug);
    set_interrupt_gate(2, (uint32_t)&NMI);
    set_trap_gate(3, (uint32_t)&breakpoint);
    set_trap_gate(4, (uint32_t)&overflow);
    set_trap_gate(5, (uint32_t)&BOUND_range);
    set_trap_gate(6, (uint32_t)&invalid_opcode);
    set_trap_gate(7, (uint32_t)&device_not_availible);
    set_trap_gate(8, (uint32_t)&double_fault);
    set_trap_gate(9, (uint32_t)&segment_overrun);
    set_trap_gate(10, (uint32_t)&invalid_TSS);
    set_trap_gate(11, (uint32_t)&segment_not_present);
    set_trap_gate(12, (uint32_t)&stack_fault);
    set_trap_gate(13, (uint32_t)&gen_protection);
    set_interrupt_gate(14, (uint32_t)&page_fault);
    //set_not_present_gate(15, (uint32_t)&no_handler);
    set_trap_gate(16, (uint32_t)&x86_FP);
    set_trap_gate(17, (uint32_t)&alignment_check);
    set_trap_gate(18, (uint32_t)&machine_check);
    set_trap_gate(19, (uint32_t)&SIMD_FP);   

    //set_interrupt_gate(33, (uint32_t)&no_handler);
    //set_interrupt_gate(40, (uint32_t)&no_handler);
    set_interrupt_gate(33, (uint32_t)&key_handler_linkage);
    set_interrupt_gate(40, (uint32_t)&rtc_handler_linkage);
    //set_syscall_gate(128, (uint32_t)&system_call_linkage);
    set_syscall_gate(128, (uint32_t)&syscall_handler);

};

//exception handlers
void divide_error(){
    cli();      //prevent further interrupts from occuring
    printf("Divide Error Exception (#DE) \n");
    while(1);
};     
void debug(){
    cli();      //prevent further interrupts from occuring
    printf("Debug Exception (#DB) \n");
    while(1);
};            
void NMI(){
    cli();      //prevent further interrupts from occuring
    printf("Nonmaskable Interrupt (NMI) \n");
    while(1);
};              
void breakpoint(){
    cli();      //prevent further interrupts from occuring
    printf("Breakpoint Exception (#BP) \n");
    while(1);
};       
void overflow(){
    cli();      //prevent further interrupts from occuring
    printf("Overflow Exception (#OF) \n");
    while(1);
};         
void BOUND_range(){
    cli();      //prevent further interrupts from occuring
    printf("BOUND Range Exceeded Exception (#BR) \n");
    while(1);
};        
void invalid_opcode(){
    cli();      //prevent further interrupts from occuring
    printf("Invalid Opcode Exception (#UD) \n");
    while(1);
};   
void device_not_availible(){
    cli();      //prevent further interrupts from occuring
    printf("Device Not Available Exception (#NM) \n");
    while(1);
};
void double_fault(){
    cli();      //prevent further interrupts from occuring
    printf("Double Fault Exception (#DF) \n");
    while(1);
};     
void segment_overrun(){
    cli();      //prevent further interrupts from occuring
    printf("Coprocessor Segment Overrun \n");
    while(1);
};  
void invalid_TSS(){
    cli();      //prevent further interrupts from occuring
    printf("Invalid TSS Exception (#TS) \n");
    while(1);
};      
void segment_not_present(){
    cli();      //prevent further interrupts from occuring
    printf("Segment Not Present (#NP) \n");
    while(1);
};
void stack_fault(){
    cli();      //prevent further interrupts from occuring
    printf("Stack Fault Exception (#SS) \n");
    while(1);
};      
void gen_protection(){
    cli();      //prevent further interrupts from occuring
    printf("General Protection Exception (#GP) \n");
    while(1);
};   
void page_fault(){
    cli();      //prevent further interrupts from occuring
    printf("Page-Fault Exception (#PF) \n");
    while(1);
};       
void x86_FP(){
    cli();      //prevent further interrupts from occuring
    printf("x87 FPU Floating-Point Error (#MF) \n");
    while(1);
};           
void alignment_check(){
    cli();      //prevent further interrupts from occuring
    printf("Alignment Check Exception (#AC) \n");
    while(1);
};  
void machine_check(){
    cli();      //prevent further interrupts from occuring
    printf("Machine-Check Exception (#MC) \n");
    while(1);
};    
void SIMD_FP(){
    cli();      //prevent further interrupts from occuring
    printf("SIMD Floating-Point Exception (#XF) \n");
    while(1);
};          


void system_call(){
    //cli();      //prevent further interrupts from occuringprintf("System call was called \n");
    printf("Sytem call called\n");
    //while(1);
};
void no_handler(){
    cli();      //prevent further interrupts from occuring
    printf("no handler \n");
    while(1);
};
