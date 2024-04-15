#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "rtc.h"
#include "i8259.h"
#include "keyboard.h"
#include "file.h"
#include "syscall_helper.h"
#include "terminal.h"

#define PASS 1
#define FAIL 0

//test :3

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 15; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
				//printf("fail\n");
			assertion_failure();
			result = FAIL;
		}
	}
	for (i = 16; i < 20; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
				//printf("fail\n");
			assertion_failure();
			result = FAIL;
		}
	}
	if ((idt[33].offset_15_00 == NULL) && 
			(idt[33].offset_31_16 == NULL)){
				//printf("fail\n");
			assertion_failure();
			result = FAIL;
	}
	if ((idt[40].offset_15_00 == NULL) && 
			(idt[40].offset_31_16 == NULL)){
				//printf("fail\n");
			assertion_failure();
			result = FAIL;
	}
	if ((idt[128].offset_15_00 == NULL) && 
			(idt[128].offset_31_16 == NULL)){
				//printf("fail\n");
			assertion_failure();
			result = FAIL;
	}

	return result;
}

/* 
 * idt_div0_test
 * Raises the Divide Error Exception (#DE) (aka divide by 0) exception  
 * Inputs: None
 * Outputs: print the divide error text from the handler
 * Side Effects: None
 * Coverage: divide error exception handler
 * Files: idt.c
 */
void idt_div0_test(){

	int i = 1;
	int j = 0;
	int k;

	k = i/j;
	//asm volatile("int $0");
};

void debug_test(){
	asm volatile("int $1");
}

void NMI_test(){
	asm volatile("int $2");
};

void invalid_opcode_test(){
	asm volatile("int $6");
};

void stack_fault_test(){
	asm volatile("int $12");
};

void x86_FP_test(){
	asm volatile("int $16");
};

void key_test(){
};



void RTC_test(){
	asm volatile("int $40");
};


/*
	Tests for paging memory allocation:
	A page fault exception will occur if trying to write outside of video memory or kernel memory.
	Otherwise, no exception will be raised.
*/

void mem_test_video_inside_1(){
	int8_t *ptr;
	ptr = (int8_t*)0xB8FFF;
	*ptr = 5;
}

void mem_test_video_inside_2(){
	int8_t *ptr;
	ptr = (int8_t*)0xB8000;
	*ptr = 5;
}

void mem_test_video_inside_3(){
	int8_t *ptr;
	ptr = (int8_t*)0xB8CCC;
	*ptr = 5;
}

void mem_test_video_outside_1(){
	int8_t *ptr;
	ptr = (int8_t*)0xB9000;
	*ptr = 5;
}

void mem_test_video_outside_2(){
	int8_t *ptr;
	ptr = (int8_t*)0xB7FFF;
	*ptr = 5;
}

void mem_test_video_outside_3(){
	int8_t *ptr;
	ptr = (int8_t*)0x10000;
	*ptr = 5;
}

void mem_test_video_outside_4(){
	int8_t *ptr;
	ptr = (int8_t*)0xC0000;
	*ptr = 5;
}

void mem_test_kernel_inside_1(){
	int8_t *ptr;
	ptr = (int8_t*)0x400000;
	*ptr = 5;
}

void mem_test_kernel_inside_2(){
	int8_t *ptr;
	ptr = (int8_t*)0x7FFFFF;
	*ptr = 5;
}

void mem_test_kernel_inside_3(){
	int8_t *ptr;
	ptr = (int8_t*)0x6CCCCC;
	*ptr = 5;
}

void mem_test_kernel_outside_1(){
	int8_t *ptr;
	ptr = (int8_t*)0x800000;
	*ptr = 5;
}

void mem_test_kernel_outside_2(){
	int8_t *ptr;
	ptr = (int8_t*)0x3FFFFF;
	*ptr = 5;
}

void mem_test_kernel_outside_3(){
	int8_t *ptr;
	ptr = (int8_t*)0x900000;
	*ptr = 5;
}

void mem_test_choose_address(int addr){
	int8_t *ptr;
	ptr = (int8_t*)addr;
	*ptr = 5;
}

void mem_test_null_pointer(){
	int8_t *ptr;
	ptr = NULL;
	*ptr = 5;
}

void mem_test_change_memory(int addr){
	int8_t *ptr;
	ptr = (int8_t*)addr;
	printf("before: %x \n", *ptr);
	*ptr = 5;
	printf("after: %x \n", *ptr);
}



// add more tests here

/* Checkpoint 2 tests */

//print characters at all of the frequencies that are powers of 2 between 2 and 2024 
//tests RTC_open, RTC_close, RTC_read, RTC_write, and RTC_freqeuency
//if all functions work they should return 0 so result will be 0 if all correct,
//watching terminal display needed to fully test
int RTC_freq_RW_test(){
	TEST_HEADER;
	//go through frequencies 
	int32_t i, a;
	int32_t fd;
	int result = 0;
	int final;
	int buf;
	clear_screen();							//clear screen
	fd = RTC_open((uint8_t*)"RTC");			//open RTC
	result += fd; 
	for (i = 2; i <=1024; i*=2){			//go through all specified frequencies (powers of 2)
		buf = i;
		result += RTC_write(fd, &buf, 4);	//set frequency
		for (a = 0; a < i; a++){
			result += RTC_read(fd,0,0);		//wait until interrupt occurs
			printf("1");                //show the interrupt occured in terminal
		}
		printf("\n");						//seperate frequencies with a new line
	}
	result += RTC_close(fd);				//close RTC

	//test if everything works
	if (result != 0){					
		assertion_failure();
		final = FAIL;
	}
	else{
		final = PASS;
	}
	return final;
};


void terminal_wr_test1(){
	clear_screen();

	const uint8_t* filename;
	int32_t fd;

	int i, j;
	const char* buf = "391OS> "; 
	
	char read_buf[128];
	
	terminal_open(filename);

	terminal_write(0, buf, 7);

	i = terminal_read(0, read_buf, 128);

	j = terminal_write(0, read_buf, 128);

	terminal_close(fd);

}; 

void terminal_wr_test2(){
	clear_screen();
	// to pass, should not print the 9's in string
	// buffer size 140
	const uint8_t* filename;
	int32_t fd;

	char* buf = "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111118888here9999"; //140
	
	terminal_open(filename);

	printf("test nbytes 128 \n");
	terminal_write(0, buf, 128);

	printf("\ntest nbytes 140 \n");
	terminal_write(0, buf, 140);

	terminal_close(fd);

}; 


void terminal_wr_test3(){
	clear_screen();

	const uint8_t* filename;
	int32_t fd;

	char* buf = "EdwardOS> "; 
	
	char read_buf[128];
	
	terminal_open(filename);

	while(1){
		terminal_write(0, buf, 10);

		terminal_read(0, read_buf, 128);

		terminal_write(0, read_buf, 128);
	}
	terminal_close(fd);

}; 

void file_system_read(int8_t* name) {
	file_open((uint8_t*)name);
	char buf[cur_file_det.length];
	file_read(0, buf, 0);
	clear_screen();
    file_key_write(0, (char*)data_buffer.data, cur_file_det.length);
    printf("\nFile_name: %s", name);
}

void directory_read_test_full() {
	int i;
	char* buf;
	uint8_t* file_name;
	clear_screen();
	directory_open(file_name);
	for(i = 0; i < num_dir_entries; i++) {
		directory_read(0, buf, 0);
		printf("file name: ");
        printf("%s", cur_dir.file_name);
        printf(", file type: ");
        printf("%d", cur_dir.file_type);
        printf(", file size: ");
        printf("%d", file_size);
        printf("\n");
		directory_open(file_name);
	}
}

void directory_read_test_single() {
	clear_screen();
	char* buf;
	uint8_t* file_name;
	directory_open(file_name);
	directory_read(0, buf, 0);
	printf("file name: ");
	printf("%s", cur_dir.file_name);
	printf(", file type: ");
	printf("%d", cur_dir.file_type);
	printf(", file size: ");
	printf("%d", file_size);
	printf("\n");
	directory_open(file_name);
}

/* Checkpoint 3 tests */

void parse_arguments_test() {
	unsigned char main_buf[100] = "     hello.txt      bruh.txt  rwerhowiw   bruh";
	unsigned char filename_buf[100];
	unsigned char argument_buf[100];
	parse_arguments(main_buf, filename_buf, argument_buf);
}

void test_file_validity(int8_t* fname) {
	//uint32_t returnval = check_file_validity((uint8_t*)fname); // change 
	// if(returnval == -1) {
	// 	printf("failed! not executable");
	// } else {
	// 	printf("success! executable file");
	// }
}
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//idt_div0_test();
	//debug_test();
	//NMI_test();
	//invalid_opcode_test();
	//stack_fault_test();
	//x86_FP_test();
	//key_test();
	//while(1){
		//RTC_test();
	//}
	//mem_test_choose_address(0xB9000);
	//mem_test_null_pointer();
	//mem_test_change_memory(0xB8FFF);
	//mem_test_video_inside_1();
	//mem_test_video_inside_1();
	//mem_test_video_inside_2();
	//mem_test_video_inside_3();
	//mem_test_video_outside_1();
	//mem_test_video_outside_2();
	//mem_test_video_outside_3();
	//mem_test_video_outside_4();
	//mem_test_kernel_inside_1();
	//mem_test_kernel_inside_2();
	//mem_test_kernel_inside_3();
	//mem_test_kernel_outside_1();
	//mem_test_kernel_outside_2();
	//mem_test_kernel_outside_3();
	
	// launch your tests here

	//terminal_wr_test1();

	//terminal_wr_test2();

	/* terminal :) */
	//terminal_wr_test3();

	//TEST_OUTPUT("RTC_freq_RW_test", RTC_freq_RW_test());


	//file_system_read("frame1.txt");
	//directory_read_test_full();
	//directory_read_test_single();
	//parse_arguments_test();
	//test_file_validity("grep");

}
