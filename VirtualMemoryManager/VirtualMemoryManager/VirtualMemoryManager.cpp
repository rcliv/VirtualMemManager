// VirtualMemoryManager.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
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

typedef struct {
	frame_t page[PAGE_SIZE];
} page;

typedef struct {
	page page_table[NUM_PAGES];
} pageTable;

//Functions
int logicAdrrLoader(char * fileName, int * logicAddrList);
int extractLogicAddr(laddress_t address, page_t * pageNum, offset_t * offset);
int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum);
int searchPageTable(bool * isTlbHit, page_t pageNum, bool * isPageFault, frame_t * frameNum);
// NEEDS TO ALSO PASS IN PHYSICAL MEMORY BUT I DONT KNOW WHAT THAT MEANS
int handlePageFault(page_t pageNum, pageTable * pagetable, tlb * tlbUsed);

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



// TLB functions
int TLB_init(tlb *tlb) {
    unsigned int i;
    for (i = 0; i < TLB_SIZE; i++)
        tlb->tlb_entry[i].valid = false;
    return 0;
}

int TLB_display(tlb_t tlb) {
    unsigned int i;
    
    for (i = 0; i < TLB_SIZE; i++) {
        printf("TLB entry " + i + ", page num: " + tlb.tlb_entry[i].pageNum 
        	+ ", frame num: " + tlb.tlb_entry[i].frameNum);
        if (tlb.tlb_entry[i].valid == FALSE)
            printf("Invalid\n");
        else printf("Valide\n");
    }
    
    return 0;
}





