#include "../h/KConsole.h"
#include "../h/syscall_cpp.hpp"
void init(){
    MemoryAllocator::init();
    KConsole::init();
    TCB::init();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);    //set Riscv::supervisorTrap as Interrupt Service Routine
    Riscv::ms_sstatus(2);   //SIE
}

int main() {
    init();


    //WORK.....

    return 0;

}