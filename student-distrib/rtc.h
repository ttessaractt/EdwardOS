#include "types.h"

#define RTC_VECTOR     40  //RTC interrupt number
#define RTC_PORT1       0x70    //I/O port used to specify index or register number & disable NMI
#define RTC_PORT2       0x71    //I/O port used to read/write to/from byte of CMOS configuration space
#define REG_A           0x0A    //RTC Status register A
#define REG_B           0x0B    //RTC Status register B
#define REG_C           0x0C    //RTC Status register C

#define MAX_FREQ        1024    //max frequency in Hz
#define MIN_FREQ        2       //min frequency in Hz 


extern void RTC_init();         //RTC initialization function
extern void RTC_handler();      //RTC handler function for IDT
extern int32_t RTC_frequency(int freq);    //change RTC frequency
extern int32_t RTC_open(const uint8_t* filename);         //
extern int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t RTC_close(int32_t fd);

//control NMI interrupts
extern void NMI_enable();
extern void NMI_disable();
