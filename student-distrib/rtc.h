#define RTC_VECTOR     40  //RTC interrupt number
#define RTC_PORT1       0x70    //I/O port used to specify index or register number & disable NMI
#define RTC_PORT2       0x71    //I/O port used to read/write to/from byte of CMOS configuration space
#define REG_A           0x8A    //RTC Status register A
#define REG_B           0x8B    //RTC Status register B
#define REG_C           0x8C    //RTC Status register C

extern void RTC_init();
extern void RTC_handler();

//control NMI interrupts
extern void NMI_enable();
extern void NMI_disable();
