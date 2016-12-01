// VirtualMemoryManager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
/* offset is used to address the location within a page */
#define OFFSET_BITS 8
/* mask is used to extract the offset from a logical address */ 
#define OFFSET_MASK 0xFF 

#define NUM_PAGES 256
#define NUM_FRAMES 256
#define PAGE_SIZE 256
#define TLB_SIZE 16



/* There is no byte data type in C */
typedef unsigned char byte;

/* new data type of pages, frames, and offset */
typedef unsigned int page_t;
typedef unsigned int frame_t;
typedef unsigned int offset_t;
typedef unsigned int laddress_t;
typedef unsigned int paddress_t;

/* 
 * In C language, there is no binary format in printf 
 * You must implement the following functions to print binary format 
 * itob16() and itob8() are modified from itob() by Xiao Qin.
 */
char *itob(int x);
char *itob16(int x);
char *itob8(int x);



typedef struct {
	page_t pageNum;
	frame_t frameNum;
	bool valid;
} tlbEntry;

typedef struct {
	tlbEntry tlb_entry[TLB_SIZE];
	unsigned int next_tlb_ptr;
} tlb;

//Functions
int logicAdrrLoader(char * fileName, int * logicAddrList);
int extractLogicAddr(laddress_t address, page_t * pageNum, offset_t * offset);

int main()
{
	ifstream myfile("Text.txt");
	int32_t a, masked;
	int32_t mask;
	mask = (1 << 16) - 1;
	
	while (myfile >> a) {
		printf("\n %d", a);
		masked = a & mask;
		printf("\nMasked: %d", masked);
	}

	myfile.close();
    return 0;
}

int logicAdrrLoader(char * fileName, laddress_t * logicAddrList) {

	return 0;

}
int extractLogicAddr(laddress_t address, page_t * pageNum, offset_t * offset) {
	return 0;
}

