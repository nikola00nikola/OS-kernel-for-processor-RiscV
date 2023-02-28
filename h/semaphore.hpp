#ifndef PROJECT_BASE_V1_0_SEMAPHORE_HPP
#define PROJECT_BASE_V1_0_SEMAPHORE_HPP

#include "list.hpp"
#include "tcb.hpp"

#define sem_t Sem*

class Sem{
private:
public:
    uint32 value;
    List<TCB> waitingQueue;
    Sem(uint32 val) : value(val){

    };

    void wait();
    void signal();
    void exitSemaphore();
    friend class Riscv;

};


#endif //PROJECT_BASE_V1_0_SEMAPHORE_HPP
