// VirtualMemoryManager.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>

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
typedef vector<laddress_t> logicAddressList_t;
typedef vector<paddress_t> physAddressList_t;


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

typedef frame_t pageTable_t[NUM_PAGES];

//Functions

int logicAdrrLoader(string fileName, vector<laddress_t> * logicAddrList);
int initPageTable(pageTable_t pageTable);
int TLB_init(tlb *tlb);
int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch);
int searchPageTable(bool * isTlbHit, page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable_t* page_Table);
// NEEDS TO ALSO PASS IN PHYSICAL MEMORY BUT I DONT KNOW WHAT THAT MEANS
int handlePageFault(page_t pageNum, pageTable_t * pagetable, tlb * tlbUsed);
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
    
    page_t pageNum;
    frame_t frameNum;
    offset_t offset;
    
    // Addresses
    laddress_t logicAddress;
    paddress_t physicalAddress;
    
    /* The TLB and page table */
    tlb* tlb;
    pageTable_t pageTable;
    
    /* Simulated main memory */
    physical_memory_t physical_memory;
    
    // Address Lists
    physAddressList_t* physAddressList;
    logicAddressList_t* logicAddressList;
    
    // File Names
    const char input_file[] = "InputFile.txt";
    const char backing_store_file_name[] = "BACKING_STORE";
    
    // Initialize the tlb and page table
    TLB_init(tlb);
    initPageTable(pageTable);
    
    // Load the logical addresses from the test file
    int count = logicAdrrLoader(input_file, logicAddressList);
    
    // Dont know what this is for vv lol
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
    // ^^^^^^^^^^^^^^^^^^^^^^^^^

    return 0;
}

int logicAdrrLoader(string fileName, vector<laddress_t> * logicAddrList) {
    int count = 0;
    ifstream instream(fileName);
    instream.open(fileName);
    if(instream.fail()){
        cout << "File failed to open.";
        exit(1);
    }
    
    unsigned int nextLogicAddr;
    while (!instream.eof()) {
        instream >> nextLogicAddr;
        logicAddrList->push_back(nextLogicAddr);
        count++;
    }
    instream.close();
    return count;
}

int extractLogicAddr(laddress_t address, page_t * pageNum, offset_t * offset) {
    return 0;
}

// Page Table Initialization
int initPageTable(pageTable_t pageTable) {
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

int searchPageTable(bool * isTlbHit, page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable_t* page_Table) {
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

// Loading from backingstore
int load_frame_to_physical_memory(page_t pageNum, const char *backingStoreFileName, physical_memory_t physical_memory, frame_t *frameNum) {
    FILE *file = fopen(backingStoreFileName, "r");
    fpos_t pos;
    char one_byte;
    
    if (file == 0 ) {
        printf( "Could not open file: %s.\n", backingStoreFileName);
    }
    
    else {
        fseek(file, pageNum, SEEK_SET);
        fgetpos(file, &pos);
        frame_t frame;
        for (frame = 0; frame < 256; frame++) {
            if(physical_memory[frame] != NULL){
                break;
            }
        }
        
        for (int i = 0; i < 256; i++) {
            fread(&one_byte, 1, 1, file);
            physical_memory[frame][i] = one_byte;
        }
        
        fclose( file );
        frameNum = &frame;
    }
    return 0;
}





