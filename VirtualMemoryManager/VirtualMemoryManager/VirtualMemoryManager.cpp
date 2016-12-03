// VirtualMemoryManager.cpp : Defines the entry point for the console application.
//

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
#define FRAME_SIZE 256
#define TLB_SIZE 16
#define LOGIC_ADDR_LIST_SIZE 1024



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
    unsigned int age;
} tlbEntry;

typedef struct {
    tlbEntry tlb_entry[TLB_SIZE];
    unsigned int next_tlb_ptr;
} tlb;

typedef frame_t pageTable_t[NUM_PAGES];

//Functions

int logicAdrrLoader(string fileName, vector<laddress_t> * logicAddrList);
int extractLogicAddr(laddress_t address, page_t * pageNum, offset_t * offset);
int initPageTable(pageTable_t pageTable);
int TLB_init(tlb *tlb);
int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch);
int searchPageTable(page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable_t* page_Table);
// NEEDS TO ALSO PASS IN PHYSICAL MEMORY BUT I DONT KNOW WHAT THAT MEANS
int handlePageFault(page_t pageNum, pageTable_t * pagetable, tlb * tlbUsed);
int load_frame_to_physical_memory(page_t pageNum, const char *backingStoreFileName, physical_memory_t physical_memory, frame_t *frameNum);
int createPhysicalAddress(frame_t f_num, offset_t off, paddress_t *physical_addr);
int TLB_replacement_FIFO(page_t pageNum, frame_t frameNum, tlb *tlb);
int TLB_replacement_LRU(page_t pageNum, frame_t frameNum, tlb *tlb);

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
    
    // The TLB and page table
    tlb tlb;
    pageTable_t pageTable;
    
    // Flags to keep track of TLB and page faults
    bool tlbHit;
    bool pageFault;
    
    // Simulated main memory
    physical_memory_t physical_memory;
    
    // Address Lists
    physAddressList_t physAddressList;
    logicAddressList_t logicAddressList;
    
    // File Names
    const char input_file[] = "testinput.txt";
    const char backing_store_file_name[] = "BACKING_STORE";
    
    // Initialize the tlb and page table
    TLB_init(&tlb);
    initPageTable(pageTable);
    
    // Load the logical addresses from the test file
    int count = logicAdrrLoader(input_file, &logicAddressList);
    
    for (int i = 0; i < count; i++) {
        // Get a logical address, its pageNum, and offset
        extractLogicAddr(logicAddressList[i], &pageNum, &offset);
        
        load_frame_to_physical_memory(pageNum, backing_store_file_name, physical_memory, &frameNum);
        
        // Search the TLB
        searchTLB(&pageNum, &tlbHit, &frameNum, &tlb);
        
        if (tlbHit == true) {
            createPhysicalAddress(frameNum, offset, &physicalAddress);
        }
        
        // TLB miss
        else {
            searchPageTable(pageNum, &pageFault, &frameNum, &pageTable);
            
            // page hit
            if (pageFault == false) {
                // createPhysicalAddress(frameNum, offset, &physicalAddress);
                
                // TLB replacement methods
                if (FIFO == true) {
                    // TLB_replacement_FIFO(page_num, frame_num, &sys_tlb);
                }
                else {
                    // TLB_replacement_LRU(page_num, frame_num, &sys_tlb);
                }
            }
            
            // Page Fault
            else {
                // page_fault_handler(page_num, frame_num, &physical_memory, &page_table, &sys_tlb);
                // createPhysicalAddress(frameNum, offset, &physicalAddress);
            }
        } // End of TLB Miss
        
        // Read one-byte value from the physical memory
        // read_physical_memory(physical_address,physical_memory, &value);
        
        // Update the address-value list
        // update_address_value_list(logic_address, physical_address, value, address_value_list);
    } // End of logicAddrList
    
    // Output the address-value list into an output file
    // output_address_value_list(output_file, address_value_list);
    
    return 0;
}

int logicAdrrLoader(string fileName, vector<laddress_t> * logicAddrList) {
    int count = 0;
    ifstream instream(fileName);
    if (instream.fail()) {
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
    *pageNum = address >> OFFSET_BITS;
    *offset = address & OFFSET_MASK;
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
    tlb->next_tlb_ptr = 0;
    for (i = 0; i < TLB_SIZE; i++)
        tlb->tlb_entry[i].valid = false;
        tlb->tlb_entry[i].age = 0;
    return 0;
}

int searchTLB(page_t * pageNum, bool * isTlbHit, frame_t * frameNum, tlb * tlbSearch) {
    // Increment all ages
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb->tlb_entry[i].age++;
    }

    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlbSearch->tlb_entry[i].pageNum == *pageNum) {
            *isTlbHit = true;
            *frameNum = tlbSearch->tlb_entry[i].frameNum;
            tlbSearch->tlb_entry[i].age = 0;
            return 0;
        }
    }
    *isTlbHit = false;
    return 0;
}

int searchPageTable(page_t pageNum, bool * isPageFault, frame_t * frameNum, pageTable_t* page_Table) {
    if (*(page_Table + pageNum) == NULL) {
        *isPageFault = true;
    }
    else {
        *frameNum = *page_Table[pageNum];
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
    
    if (file == 0) {
        printf("Could not open file: %s.\n", backingStoreFileName);
    }
    
    else {
        fseek(file, pageNum, SEEK_SET);
        fgetpos(file, &pos);
        frame_t frame;
        for (frame = 0; frame < 256; frame++) {
            if (physical_memory[frame] != NULL) {
                break;
            }
        }
        
        for (int i = 0; i < 256; i++) {
            fread(&one_byte, 1, 1, file);
            physical_memory[frame][i] = one_byte;
        }
        
        fclose(file);
        frameNum = &frame;
    }
    return 0;
}

int createPhysicalAddress(frame_t f_num, offset_t off, paddress_t *physical_addr) {
    *physical_addr = f_num * FRAME_SIZE + off;
    return 0;
}

int TLB_replacement_FIFO(page_t pageNum, frame_t frameNum, tlb *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        // If the tlb isn't full yet
        if (tlb->tlb_entry[i].valid == false) {
            tlb->tlb_entry[i].pageNum = pageNum;
            tlb->tlb_entry[i].frameNum = frameNum;
            tlb->tlb_entry[i].valid = true;
            return 0;
        }
    }
    
    // If the tlb is full
    tlb->tlb_entry[tlb->next_tlb_ptr].pageNum = pageNum;
    tlb->tlb_entry[tlb->next_tlb_ptr].frameNum = frameNum;
    tlb->tlb_entry[tlb->next_tlb_ptr].valid = true;
    if(tlb->next_tlb_ptr == 15) {
        tlb->next_tlb_ptr = 0;
    }
    else {
        tlb->next_tlb_ptr++;
    }
    return 0;
}

int TLB_replacement_LRU(page_t pageNum, frame_t frameNum, tlb *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        // If the tlb isn't full yet
        if (tlb->tlb_entry[i].valid == false) {
            tlb->tlb_entry[i].pageNum = pageNum;
            tlb->tlb_entry[i].frameNum = frameNum;
            tlb->tlb_entry[i].valid = true;
            return 0;
        }
    }
    
    // Find the oldest tlb
    int oldestAge = 0;
    int oldestIndex = 0;
    for (int i = 0; i < TLB_SIZE; i++) {
        if (oldestAge < tlb->tlb_entry[i].age) {
            oldestAge = tlb->tlb_entry[i].age;
            oldestIndex = i;
        }
    }
    
    // Replace the oldest tlb
    tlb->tlb_entry[oldestIndex].pageNum = pageNum;
    tlb->tlb_entry[oldestIndex].frameNum = frameNum;
    tlb->tlb_entry[oldestIndex].age = 0;
    tlb->tlb_entry[oldestIndex].valid = true;
    
    return 0;
}



