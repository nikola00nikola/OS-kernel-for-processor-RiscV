#include "../h/semaphore.hpp"
#include "../h/riscv.hpp"

void Sem::signal() {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(2);   //SIE CLEAR
    if(this->waitingQueue.peekFirst() == nullptr)
        this->value++;
    else
        Scheduler::put(waitingQueue.removeFirst());
    Riscv::w_sstatus(sstatus);
}

void Sem::wait() {
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(2);   //SIE CLEAR
    if(this->value > 0){
        this->value--;
        Riscv::w_sstatus(sstatus);
    }
    else{
        waitingQueue.addLast(TCB::running);
        Riscv::w_sstatus(sstatus);
        TCB::dispatch(true);
    }
}

void Sem::exitSemaphore() {
    TCB* cur;
    while((cur = waitingQueue.removeFirst()) != nullptr){
        Scheduler::put(cur);
    }
}




