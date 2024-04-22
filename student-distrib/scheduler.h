#include "types.h"

extern void PIT_init();
extern void PIT_handler();
extern void PIT_link();
extern int32_t PIT_frequency(int32_t freq);
extern int32_t PIT_open(const uint8_t* filename);
extern int32_t PIT_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t PIT_write(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t PIT_close(int32_t fd);
extern void scheduler();

