#include "syscalls.h"


/*
terminates a process & returns specified value to parent process
The system call handler itself is responsible for expanding the 8-bit argument \
 from BL into the 32-bit return value to the parent program’s execute system call

 Be careful not to return all 32 bits from EBX. 

 never return to caller

*/
int32_t halt (uint8_t status){

};


/*
attempts to load and execute a new program, then hands off the processor to the new program until it terminates

command is a space seperated series of words
1st word: file name of the program to be executed 
rest of the command:
    - should have leading spaces removed 
    - then passed to new program (on request) via the getargs system call

The execute call returns:
-1: if the command cannot be executed, ex:
    - if the program does not exist
    - the filename specified is not an executable
256: if the program dies by an exception
a value in the range 0 to 255: if the program executes a halt system call, in which case the value returned is that given by the program’s call to halt.
*/
int32_t execute (const uint8_t* command){

};

/*
*/
int32_t read (int32_t fd, void* buf, int32_t nbytes){

};

/*
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes){

};


int32_t open (const uint8_t* filename){

};


int32_t close (int32_t fd){

};


int32_t getargs (uint8_t* buf, int32_t nbytes){

};


int32_t vidmap (uint8_t** screen_start){

};


int32_t set_handler (int32_t signum, void* handler){

};


int32_t sigreturn (void){

};