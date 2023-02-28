//
// Created by os on 8/29/22.extern const void* HEAP_START_ADDR;
//extern const void* HEAP_END_ADDR;
//
#include "../h/MemoryAllocator.h"
#include "../h/riscv.hpp"
uint64* MemoryAllocator::head = (uint64*) HEAP_START_ADDR;


void MemoryAllocator::init() {
    if(instance == nullptr){
        MemoryAllocator::head = (uint64*) HEAP_START_ADDR;
        uint64 size = (uint64) HEAP_END_ADDR - (uint64) HEAP_START_ADDR;
        uint64 next = 0;
        uint64 freeHead = (uint64) head;
        __asm__ volatile ("sd %[len], 0 (%[head])" : : [len] "r" (size) , [head] "r" (freeHead));
        __asm__ volatile ("sd %[next], 8 (%[head])" : : [next] "r" (next) , [head] "r" (freeHead));
    }

}

MemoryAllocator::MemoryAllocator() {

}

MemoryAllocator* MemoryAllocator::instance = nullptr;

uint64 MemoryAllocator::allocate(uint64 size) {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(2);   //SIE CLEAR

    uint64 retval=0;
    const uint32 SIZE_LENGTH = 4;
    size+= SIZE_LENGTH;
    if(size < MEM_BLOCK_SIZE)
        size = MEM_BLOCK_SIZE;
    if(size % MEM_BLOCK_SIZE != 0)
        size += MEM_BLOCK_SIZE - size%MEM_BLOCK_SIZE;   //allignment
    uint64 cur = findFirstFit(size);
    if(cur == 0){
        Riscv::w_sstatus(sstatus);
        return 0;
    }

    uint64 sizeBlck;
    __asm__ volatile ("ld %[size], 0 (%[addr])" : [size] "=r" (sizeBlck) : [addr] "r" (cur));

    if(size == sizeBlck){
        retval = cur;
        uint64 prev = findPrev(cur);
        if(prev == 0 || prev == cur){
            uint64 next;
            __asm__ volatile ("ld %[next], 8 (%[addr])" : [next] "=r" (next) : [addr] "r" (cur));
            head = (uint64*) next;
        }else{
            uint64 next;
            __asm__ volatile ("ld %[next], 8 (%[addr])" : [next] "=r" (next) : [addr] "r" (cur));
            __asm__ volatile ("sd %[next], 8 (%[addr])" : : [next] "r" (next) , [addr] "r" (prev));
        }
    }else{
        retval = cur + sizeBlck - size;
        sizeBlck-=size;
        __asm__ volatile ("sd %[size], 0 (%[addr])" : : [size] "r" (sizeBlck) , [addr] "r" (cur));
    }

    __asm__ volatile ("sw %[size], 0 (%[addr])" : : [size] "r" (size) , [addr] "r" (retval));
    retval+= SIZE_LENGTH;
    Riscv::w_sstatus(sstatus);
    return retval;
}

uint64 MemoryAllocator::findFirstFit(uint64 sizeBytes) {
    uint64 cur = (uint64) head;
    uint64 size;
    uint64 next;
    __asm__ volatile ("ld %[size], 0 (%[addr])" : [size] "=r" (size) : [addr] "r" (cur));
    __asm__ volatile ("ld %[next], 8 (%[addr])" : [next] "=r" (next) : [addr] "r" (cur));
    while(next != 0 && size < sizeBytes){
        cur = next;
        __asm__ volatile ("ld %[size], 0 (%[addr])" : [size] "=r" (size) : [addr] "r" (cur));
        __asm__ volatile ("ld %[next], 8 (%[addr])" : [next] "=r" (next) : [addr] "r" (cur));
    }

    if(size >= sizeBytes){
        return cur;
    }
    return 0;
}

uint64 MemoryAllocator::findPrev(uint64 address) {
    uint64 cur = (uint64) head;
    if(cur > address){
        return 0;
    }
    uint64 next;
    __asm__ volatile ("ld %[next], 8 (%[addr])" : [next] "=r" (next) : [addr] "r" (cur));
    while(next != 0 && next < address){
        cur = next;
        __asm__ volatile ("ld %[next], 8 (%[addr])" : [next] "=r" (next) : [addr] "r" (cur));
    }

    return cur;
}

int MemoryAllocator::free(void *pointer) {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(2);   //SIE

    const uint32 SIZE_LENGTH = 4;
    uint64 addr = (uint64) pointer;
    uint64 freeN;
    addr-= SIZE_LENGTH;
    if(addr < (uint64)HEAP_START_ADDR || addr + SIZE_LENGTH >= (uint64)HEAP_END_ADDR){
        Riscv::w_sstatus(sstatus);
        return -3;
    }
    uint64 sizeAllocated;
    __asm__ volatile("lw %[size], 0 (%[address])" : [size] "=r" (sizeAllocated) : [address] "r" (addr));
    if(addr + sizeAllocated > (uint64)HEAP_END_ADDR){
        Riscv::w_sstatus(sstatus);
        return -4;
    }
    uint64 freeP = findPrev(addr);

    if( freeP == 0){
        uint64 tmp = (uint64) head;
        freeN = tmp;
        if(freeN!=0 && freeN < addr + sizeAllocated) {
            Riscv::w_sstatus(sstatus);
            return -1;
        }
        head = (uint64*) addr;
        __asm__ volatile ("sd %[len], 0 (%[head])" : : [len] "r" (sizeAllocated) , [head] "r" (addr));
        __asm__ volatile ("sd %[next], 8 (%[head])" : : [next] "r" (tmp) , [head] "r" (addr));
    }else {
        uint64 sizeF ;
        __asm__ volatile ("ld %[sizeF], 0 (%[freeP])" : [sizeF] "=r" (sizeF) : [freeP] "r" (freeP));
        __asm__ volatile ("ld %[freeN], 8 (%[freeP])" : [freeN] "=r" (freeN) : [freeP] "r" (freeP));
        if(freeN!=0 && freeN < addr + sizeAllocated) {
            Riscv::w_sstatus(sstatus);
            return -1;
        }
        if(freeP!=0 && freeP + sizeF > addr) {
            Riscv::w_sstatus(sstatus);
            return -2;
        }
        if(freeP + sizeF == addr){
            sizeAllocated+= sizeF;
            __asm__ volatile ("sd %[len], 0 (%[free])" : : [len] "r" (sizeAllocated) , [free] "r" (freeP));
            addr = freeP;
        }else {
            uint64 tmpNext;
            __asm__ volatile ("ld %[tmpNext], 8 (%[freeP])" : [tmpNext] "=r" (tmpNext) : [freeP] "r" (freeP));
            __asm__ volatile ("sd %[next], 8 (%[free])" : : [next] "r" (addr) , [free] "r" (freeP));
            __asm__ volatile ("sd %[len], 0 (%[addr])" : : [len] "r" (sizeAllocated) , [addr] "r" (addr));
            __asm__ volatile ("sd %[next], 8 (%[addr])" : : [next] "r" (tmpNext) , [addr] "r" (addr));
        }
    }
    if(freeN == 0){
        __asm__ volatile ("sd %[next], 8 (%[addr])" : : [next] "r" (freeN) , [addr] "r" (addr));
    }else{
        if(addr + sizeAllocated == freeN){
            uint64 tmpNext, tmpSize;
            __asm__ volatile ("ld %[tmpNext], 8 (%[freeP])" : [tmpNext] "=r" (tmpNext) : [freeP] "r" (freeN));
            __asm__ volatile ("ld %[tmpNext], 0 (%[freeP])" : [tmpNext] "=r" (tmpSize) : [freeP] "r" (freeN));
            sizeAllocated+= tmpSize;
            __asm__ volatile ("sd %[len], 0 (%[addr])" : : [len] "r" (sizeAllocated) , [addr] "r" (addr));
            __asm__ volatile ("sd %[next], 8 (%[addr])" : : [next] "r" (tmpNext) , [addr] "r" (addr));
        }  
    }

    Riscv::w_sstatus(sstatus);
    return 0;
}