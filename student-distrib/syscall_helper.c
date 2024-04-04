/* syscall_helper.c - Helper functions for syscalls
 * vim:ts=4 noexpandtab
 */

#include "syscall_helper.h"

#include "lib.h"


int32_t execute(const uint8_t* command){

    // if (command == NULL){
    //     return -1;
    // }

    // char file_name[32+1]; // 32+1?
    // char arguments[128]; // check lengths?

    // parse_arguments(command, file_name, arguments);
    return -1;

}


int32_t parse_arguments(char* buf, char* file_name, char* arguments){

    /* return -1 for fail */
    if (buf == NULL){
        return -1;
    }

    /* filter out spaces from first file name */
    int cur_idx = 0;
    while(buf[cur_idx] == 0x20) {
        cur_idx++;
    }

    /* at first file name */
    int i = 0;
    while(buf[cur_idx] != 0x20) {
        file_name[i] = buf[cur_idx];
        i++;
        cur_idx++;
    }
    file_name[i] = '\0';
    
    /* filter out spaces between first file name and next */
    while(buf[cur_idx] == 0x20) {
        cur_idx++;
    }

    /* copy to arguments everything past first file name + spaces */
    int j;
    int l = 0;
    for(j = cur_idx; j < strlen(buf); j++) {
        arguments[l] = buf[j];
        l++;
    }
    arguments[l] = '\0';

    /* testing */
    printf("\n");
    printf("%s", file_name);
    printf("\n");
    printf("%s", arguments);

    return 1; //success

}

