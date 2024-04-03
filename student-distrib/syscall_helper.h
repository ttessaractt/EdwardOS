/* syscall_helper.h - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "types.h"


int32_t execute(const uint8 t* command);

/* need to pass in file name and argument buffer of proper size */
int32_t parse_arguments(char* buf, char* file_name, char* arguments);