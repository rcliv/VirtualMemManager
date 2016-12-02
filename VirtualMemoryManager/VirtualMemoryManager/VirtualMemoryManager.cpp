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
typedef char physical_memory_t[NUM_FRAMES][PAGE_SIZE];


/*
 * In C language, there is no binary format in printf
 * You must implement the following functions to print binary format
 * itob16() and itob8() are modified from itob() by Xiao Qin.
 */
char *itob(int x);
char *itob16(int x);
char *itob8(int x);

// Flags used to determine user's choice
bool displayAddr;
bool FIFO;

typedef struct {
    page_t pageNum;
    frame_t frameNum;
    bool valid;
} tlbEntry;

typedef struct {
    tlbEntry tlb_entry[TLB_SIZE];
    unsigned int next_tlb_ptr;
} tlb;

typedef frame_t pageTable[NUM_PAGES];

//Functions

int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch);
int searchPageTable(bool * isTlbHit, page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable * page_Table);
// NEEDS TO ALSO PASS IN PHYSICAL MEMORY BUT I DONT KNOW WHAT THAT MEANS
int handlePageFault(page_t pageNum, pageTable * pagetable, tlb * tlbUsed);
int load_frame_to_physical_memory(page_t pageNum, const char *backingStoreFileName, physical_memory_t physical_memory, frame_t *frameNum);

int main()
{
    cout << "Welcome to Group 1's VM Simulator Version 1.0\n\n"
    << "Number of logical pages: " << NUM_PAGES << endl
    << "Page size: " << PAGE_SIZE << " bytes" << endl
    << "Page table size: " << NUM_FRAMES << endl
    << "TLB size: " << TLB_SIZE << endl
    << "Number of physical frames: " << NUM_FRAMES << endl
    << "Physical memory size: 65,536 bytes\n\n";
    
    string displayAddrChoice;
    
    cout << "Display physical addresses? [y or n] ";
    cin >> displayAddrChoice;
    if (displayAddrChoice == "y" || displayAddrChoice == "Y") {
        displayAddr = true;
    }
    else {
        displayAddr = false;
    }
    
    int strategyChoice;
    
    cout << "Choose TLB Replacement Strategy [1: FIFO, 2: LRU] ";
    cin >> strategyChoice;
    if (strategyChoice == 1) {
        FIFO = true;
    }
    else {
        FIFO = false;
        
    }
    
    
    
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

// Page Table Initialization
int init_page_table(pageTable pageTable){
    for (int i = 0; i < NUM_PAGES; i++) {
        pageTable[i] = NULL;
    }
    return 0;
}

// TLB functions
int TLB_init(tlb *tlb) {
    unsigned int i;
    for (i = 0; i < TLB_SIZE; i++)
        tlb->tlb_entry[i].valid = false;
    return 0;
}

int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch) {
    for(int i = 0; i < TLB_SIZE; i++) {
        if(tlbSearch->tlb_entry[i].pageNum == *pageNum) {
            *isTlbHit = true;
            *frameNum = tlbSearch->tlb_entry[i].frameNum;
            return 0;
        }
    }
    *isTlbHit = false;
    return 0;
}

int searchPageTable(bool * isTlbHit, page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable* page_Table) {
    if(!isTlbHit) {
        if(*(page_Table + pageNum) == NULL) {
            *isPageFault = true;
        }
        else {
            *frameNum = *page_Table[pageNum];
        }
    }
    return 0;
}

int TLB_display(tlb * tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        cout << "TLB entry " << i << ", page num: " << tlb->tlb_entry[i].pageNum
        << ", frame num: " << tlb->tlb_entry[i].frameNum;
        if (tlb->tlb_entry[i].valid == false)             printf("Invalid\n");
        else printf("Valide\n");
    }
    
    return 0;
}
