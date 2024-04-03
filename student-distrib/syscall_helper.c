/* syscall_helper.c - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "syscall_helper.h"

#include "lib.h"


int32_t execute(const uint8_t* command){

    if (command == NULL){
        return -1;
    }

    char file_name[32+1]; // 32+1?
    char arguments[128]; // check lengths?

    parse_arguments(command, file_name, arguments);
    

}


int32_t parse_arguments(char* buf, char* file_name, char* arguments){

    /* return -1 for fail */

    int i = 0;
    int counter_file = 0;
    int counter_arg = 0;
    

    if (buf == NULL){
        return -1;
    }
    
    while(buf[i] != 0x20){
        counter_file++;
        i++;
        printf("%d", counter_file);
    }

    i++; // skip the space

    printf("\n");
    while(i < strlen(buf)){
        counter_arg++;
        i++;
        printf("%d", counter_arg);
    }

    for(int j = 0; j < strlen(buf); j++){
        //printf("%d\n", j);
        if (j < counter_file){
            file_name[j] = buf[j];
        }
        else if ((j > counter_file)){
            arguments[j-counter_file-1] = buf[j];
        }
    }
    file_name[counter_file] = '\0'; // mark end
    arguments[counter_arg] = '\0';
    printf("\n");
    for(i = 0; i < counter_file; i++){
        printf("%c", file_name[i]);
    }
    printf("\n");
    for(i = 0; i < counter_arg; i++){
        printf("%c", arguments[i]);
    }

    return 1; //success

}