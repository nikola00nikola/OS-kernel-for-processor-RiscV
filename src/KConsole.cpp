#include "../h/KConsole.h"
#include "../lib/hw.h"
#include "../h/syscall_c.h"

ConsoleBuffer* KConsole::inBuffer ;
ConsoleBuffer* KConsole::outBuffer;
Sem* KConsole::sem1;
Sem* KConsole::sem2;

void KConsole::__putc(char chr) {
    outBuffer->empty->wait();
    outBuffer->mutex->wait();
    outBuffer->niz[ outBuffer->tail ] = chr;
    outBuffer->tail = (outBuffer->tail + 1) % ConsoleBuffer::CBUFF_SIZE;
    outBuffer->mutex->signal();
    outBuffer->full->signal();
}

char KConsole::__getc() {
    inBuffer->full->wait();
    inBuffer->mutex->wait();
    char c=inBuffer->niz[ inBuffer->head ];
    inBuffer->head = (inBuffer->head + 1) % ConsoleBuffer::CBUFF_SIZE;
    inBuffer->count--;
    inBuffer->mutex->signal();
    inBuffer->empty->signal();
    return c;
}

//console controller sent a char, so put it in inBuffer
void KConsole::console_handler() {
    int irq = plic_claim();
    if(irq == CONSOLE_IRQ){
        char* addrStatus = (char*)CONSOLE_STATUS;

        if(*addrStatus & CONSOLE_RX_STATUS_BIT){
            while(*addrStatus & CONSOLE_RX_STATUS_BIT){
                if(inBuffer->count == inBuffer->cap)
                    break;
                putCharToInBuffer( *((char*) CONSOLE_RX_DATA));
            }
        }
        }
    plic_complete(irq);
    }

void KConsole::init() {
    inBuffer =(ConsoleBuffer*) MemoryAllocator::allocate(sizeof(ConsoleBuffer));
    inBuffer->count = 0;
    outBuffer =(ConsoleBuffer*) MemoryAllocator::allocate(sizeof(ConsoleBuffer));
    inBuffer->initEmpty();
    outBuffer->initEmpty();
    sem1 = (Sem*) MemoryAllocator::allocate(sizeof(Sem));
    sem1->value = 0;
    sem1->waitingQueue.setEmpty();
    sem2 = (Sem*) MemoryAllocator::allocate(sizeof(Sem));
    sem2->value = 0;
    sem2->waitingQueue.setEmpty();
    void* stack = (void*) MemoryAllocator::allocate(sizeof(uint64) * TCB::STACK_SIZE);
    TCB::createThread(KConsole::workerKernel, nullptr, stack, true, true);
}

//workerThread keeps transfering characters from outBuffer to console output
void KConsole::workerKernel(void *) {
    char* addrStatus = (char*)CONSOLE_STATUS;
    char* addrTX = (char*) CONSOLE_TX_DATA;

    while(true){
        //sem_wait(sem1);
        int cnt = 1000;
        while(*addrStatus & CONSOLE_TX_STATUS_BIT) {
            sem_wait(outBuffer->full);
            sem_wait(outBuffer->mutex);
            char c = outBuffer->niz[outBuffer->head];
            *addrTX = c;
            outBuffer->head = (outBuffer->head + 1) % ConsoleBuffer::CBUFF_SIZE;
            sem_signal(outBuffer->mutex);
            sem_signal(outBuffer->empty);
            if(--cnt <= 0){
                time_sleep(1);
                break;
            }
        }
    }
}

void KConsole::putCharToInBuffer(char c) {
    inBuffer->empty->wait();
    inBuffer->mutex->wait();
    inBuffer->niz[ inBuffer->tail ] = c;
    inBuffer->tail = (inBuffer->tail + 1) % ConsoleBuffer::CBUFF_SIZE;
    inBuffer->count++;
    inBuffer->mutex->signal();
    inBuffer->full->signal();
}

