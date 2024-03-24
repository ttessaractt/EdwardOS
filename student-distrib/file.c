#include "types.h"
#include "file.h"

//return 0 for success, -1 for failure

unsigned long addr;

/*
 *  
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t file_open(const uint8_t* fnamme, dentry_t* dentry){
    read_dentry_by_name(fnamme, dentry);
    return 0;
}

/*
 *  
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t file_close(){
    return 0;
}

/*
 *  file_read
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t file_read(const uint8_t* fname, dentry_t* dentry){

    return 0;
}

/*
 *  file_write
 *  Description: write to file (does nothing)
 *  Input:
 *  Output: -1
 *  Return Value: -1
 *  Side Effects: 
*/
uint32_t file_write(){
    return -1;
}




/*
 *  
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t directory_open(){
    return 0;
}

/*
 *  
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t directory_close(){
    return 0;
}

/*
 *  
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t directory_read(){

    return 0;
}

/*
 *  
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t directory_write(){
    return 0;
}

/*
 *  read_dentry_by_name
 *  Description: 
 *  Input: 
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
    //if file name is invalid
    if(fname == NULL){ return -1; }

    //check fname == file_name in dentry
    int counter = 0;
    while(*(fname + counter) != NULL){
        if(dentry->file_name[counter] != *(fname + counter)){ return -1; }
        ++counter;
    }

    return 0;
}

/*
 *  read_dentry_by_index
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
    dentry->inode_num = index;
    return 0;
    
}

/*
 *  
 *  Description: 
 *  Input:
 *  Output: 
 *  Return Value: 
 *  Side Effects: 
*/
uint32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

    return 0;
}
