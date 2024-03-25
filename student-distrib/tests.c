#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt.h"
#include "rtc.h"
#include "i8259.h"

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
	asm volatile("int $33");
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

//print characters at all of the frequencies they tell us to do
//powers of 2 between 2 and 2024 
void RTC_freq_RW_test(){
	//go through frequencies 
	int32_t i, a;
	int32_t fd;
	const void* buf;
	clear_screen();
	fd = RTC_open((uint8_t*)"RTC");
	if (fd != 0){
		printf("error!\n");
	}
	for (i = 2; i <=1024; i*=2){
		buf = (void*)i;
		RTC_write(fd, buf, 4);
		for (a = 0; a < i; a++){
			RTC_read(fd,0,0);
		}
		printf("\n");
	}


	
};


void test2(){
	clear_screen();
RTC_read(0,0,0);
RTC_read(0,0,0);
RTC_read(0,0,0);
RTC_read(0,0,0);
RTC_read(0,0,0);

}
/* Checkpoint 3 tests */
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
	//clear();
	//RTC_test();
	//test2();
	RTC_freq_RW_test();
	//RTC_freq_RW_test();
	//while(1){
		//RTC_test();
	//}
	//int i;
	//for (i = 0; i < 2000; i++){
	//	RTC_test();
	//}
	//clear_screen();
	//RTC_freq_RW_test();
	
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
}
