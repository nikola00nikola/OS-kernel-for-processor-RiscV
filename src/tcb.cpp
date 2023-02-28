#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/scheduler.hpp"
#include "../h/MemoryAllocator.h"

TCB* TCB::running=nullptr;
TCB* TCB::bootloop=nullptr;
uint64 TCB::timeSliceCounter = 0;
List<TCB::THEADER> TCB::sleepingThreads;

TCB *TCB::createThread(Body body, void* args, void* stack, bool putToSch, bool priority) {
    uint64* s = (uint64*) stack;
    TCB* t = (TCB*) MemoryAllocator::allocate(sizeof(TCB));
    t->context.ra = (uint64) &threadWraper;
    t->context.sp = stack == nullptr ? 0 : (uint64)&s[STACK_SIZE];
    t->stack = (uint64*) stack;
    t->body = body;
    t->arg = args;
    t->timeSlice = TIME_SLICE;
    t->finished = false;
    t->priority = priority;
    if(putToSch)
        Scheduler::put(t);

    return t;
    //return new TCB(body, args, stack);
}

void TCB::yield() {
    /*Riscv::pushRegisters();

    dispatch();

    Riscv::popRegisters();*/
    uint64 tmp = 0x13UL;
    __asm__ volatile ("mv a0, %[tmp]" : : [tmp] "r" (tmp));
    __asm__ volatile ("ecall");
}

void TCB::dispatch(bool dontContinueOld) {
    TCB* old=TCB::running;
    if(old->isFinished()){
        if(old->stack != nullptr){
            MemoryAllocator::free(old->stack);
        }
        MemoryAllocator::free(old);
    }
    else if(!dontContinueOld && (uint64) old != (uint64) bootloop){ Scheduler::put(old);}
    TCB::running = Scheduler::get();
    if(running == nullptr)
        running = bootloop;
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWraper() {
    Riscv::popFromStatusToCpu();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

void TCB::init() {
    TCB::running = TCB::createThread(nullptr, nullptr, nullptr, false, true);
    TCB::bootloop = TCB::createThread(TCB::funcBootLoop, nullptr,
                      (void*) MemoryAllocator::allocate(sizeof(uint64) * STACK_SIZE), false);
}

void TCB::sleep(uint64 sleepTime) {
    if(sleepTime == 0)
        return;
    putToSleep(sleepTime);
    TCB::dispatch(true);
}

void TCB::putToSleep(uint64 time) {
    THEADER* header= (THEADER*) MemoryAllocator::allocate(sizeof(THEADER));
    header->thread = running;
    header->sleepTime = time;
    if(sleepingThreads.head == nullptr)
        sleepingThreads.addLast( header);
    else if(sleepingThreads.head->data->sleepTime <= time){
        List<THEADER>::Elem* prev = sleepingThreads.head;
        header->sleepTime-= prev->data->sleepTime;
        List<THEADER>::Elem* next = prev->next;
        while(next != nullptr && next->data->sleepTime <= header->sleepTime){
            prev = next;
            next = next->next;
        }
        List<THEADER>::Elem* newElem = (List<THEADER>::Elem*) MemoryAllocator::allocate(sizeof(List<THEADER>::Elem));
        newElem->next = next;
        if(next == nullptr)
            sleepingThreads.tail = newElem;
        newElem->data = header;
        prev->next = newElem;
    }else {
        List<THEADER>::Elem* cur = sleepingThreads.head;
        uint64 OLDoffs = cur->data->sleepTime, NEWoffs = header->sleepTime;

        List<THEADER>::Elem* elem = (List<THEADER>::Elem*) MemoryAllocator::allocate(sizeof(List<THEADER>::Elem));
        elem->data = header;
        elem->next = sleepingThreads.head;
        sleepingThreads.head=elem;
        if(sleepingThreads.tail == nullptr)
            sleepingThreads.tail=sleepingThreads.head;

        cur->data->sleepTime -= NEWoffs;
        cur = cur->next;
        while(cur != nullptr){
            cur->data->sleepTime+= OLDoffs - NEWoffs;
            cur = cur->next;
        }
    }
}

void TCB::tick() {
    if(sleepingThreads.head == nullptr)
        return;

    sleepingThreads.head->data->sleepTime--;
    bool changed = (sleepingThreads.head!= nullptr && sleepingThreads.head->data->sleepTime == 0);
    while(sleepingThreads.head!= nullptr && sleepingThreads.head->data->sleepTime == 0){
        Scheduler::put(sleepingThreads.head->data->thread);
        MemoryAllocator::free(sleepingThreads.head->data);
        sleepingThreads.removeFirst();
    }
    if(changed){
        List<THEADER>::Elem* cur = sleepingThreads.head;
        if( cur == nullptr)
            return;
        uint64 startTime = cur->data->sleepTime;
        cur = cur->next;
        while(cur != nullptr){
            cur->data->sleepTime-= startTime;
            cur = cur->next;
        }
    }

}

void TCB::funcBootLoop(void* ) {
    while(true){

    }
}

