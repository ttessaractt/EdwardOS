//header file for IDT

#define ONE         0x1       //one (for present & reserved bits)
#define ZERO        0x0       //0 (for reserved bits)
#define DPL_ZERO    0x00
#define DPL_THREE   0x03
#define EIGHTBIT        0x00000000

/*#define MAKE_EXCEPTION      (name, num) \
void name(){    \

}
*/

#define RTC_VECTOR          40

void set_trap_gate(uint32_t num, uint32_t addr);           //add trap gate to IDT
void set_interrupt_gate(uint32_t num, uint32_t addr);      //add interrupt gate to IDT
void set_syscall_gate(uint32_t num, uint32_t addr);        //add gate for system calls to IDT

//initilize IDT
extern void idt_init();

extern void divide_error();     //interrupt 0
extern void debug();            //interrupt 1
extern void NMI();              //interrupt 2
extern void breakpoint();       //interrupt 3
extern void overflow();         //interrupt 4
extern void BOUND_range();      //interrupt 5
extern void invalid_opcode();   //interrupt 6
extern void device_not_availible();     //interrupt 7
extern void double_fault();     //interrupt 8
extern void segment_overrun();  //interrupt 9
extern void invalid_TSS();      //interrupt 10
extern void segment_not_present();      //interrupt 11
extern void stack_fault();      //interrupt 12
extern void gen_protection();   //interrupt 13
extern void page_fault();       //interrupt 14
extern void x86_FP();           //interrupt 16
extern void alignment_check();  //interrupt 17
extern void machine_check();    //interrupt 18
extern void SIMD_FP();          //interrupt 19

extern void system_call();      //system call (interrupt 128, 0x80)
extern void no_handler();       //no handler

void rtc_handler_linkage();
void key_handler_linkage();
//void system_call_linkage();

