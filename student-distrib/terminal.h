/* terminal.h - Functions for terminal
 * vim:ts=4 noexpandtab
 */

#include "types.h"


// terminal_open
int32_t terminal_open();

// terminal_read
int32_t terminal_read(int32_t fd, char* buf, int32_t nbytes);

// terminal_write
int32_t terminal_write(int32_t fd, char* buf, int32_t nbytes);

// terminal_close
int32_t terminal_close();
