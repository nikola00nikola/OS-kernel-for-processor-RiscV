#ifndef _console_buffer_cpp_
#define _console_buffer_cpp_

#include "../h/semaphore.hpp"

class ConsoleBuffer{
public:
    static const int CBUFF_SIZE = 124;
    int cap;
    int head, tail, count;
    Sem* full, *empty, *mutex;
    char* niz;
    void initEmpty() {
        this->cap = CBUFF_SIZE;
        full = (Sem*) MemoryAllocator::allocate(sizeof(Sem));
        full->value = 0;
        full->waitingQueue.setEmpty();
        empty = (Sem*) MemoryAllocator::allocate(sizeof(Sem));
        empty->value = CBUFF_SIZE;
        empty->waitingQueue.setEmpty();
        mutex = (Sem*) MemoryAllocator::allocate(sizeof(Sem));
        mutex->value = 1;
        mutex->waitingQueue.setEmpty();
        niz=(char*) MemoryAllocator::allocate(sizeof(char) * CBUFF_SIZE);
        head = 0;
        tail = 0;
    }
};




#endif