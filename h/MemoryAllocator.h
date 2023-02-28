//
// Created by os on 8/29/22.
//

#ifndef PROJECT_BASE_V1_0_MEMORYALLOCATOR_H
#define PROJECT_BASE_V1_0_MEMORYALLOCATOR_H

#include "../lib/hw.h"

extern const void* HEAP_START_ADDR;
extern const void* HEAP_END_ADDR;

class MemoryAllocator{
private:
    MemoryAllocator();
    static MemoryAllocator* instance;
    static uint64* head;
    static uint64 findFirstFit(uint64 sizeBytes);
    static uint64 findPrev(uint64 address);

public:
    static void init();
    static uint64 allocate(uint64 size);

    static int free(void* pointer);
    //static void printFreeList();
};


#endif //PROJECT_BASE_V1_0_MEMORYALLOCATOR_H
