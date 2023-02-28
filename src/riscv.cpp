#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/MemoryAllocator.h"
#include "../h/KConsole.h"

void Riscv::handleSupervisorTrap(){
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a5, 0 (sp)");     //OLDSP
    uint64 scause = r_scause();
    //scause - CAUSE register of interrupt

    if(scause == (0x09UL) || scause == (0x08UL)){
        //System call from s/u mode
        //System call operation code is stored in register a0
        //System call parameters are stored registers a1, a2...
        //System call return value through register a0
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();
        uint64 oldSP;
        __asm__ volatile ("ld %[a], 0 (sp)" : [a] "=r" (oldSP));
        uint64 a0;
        __asm__ volatile ("ld %[a], 10 * 8 ( %[sp])" : [a] "=r" (a0) : [sp] "r" (oldSP)); //x10 = a0
        switch (a0) {
            case 0x13UL: {
                //void thread_dispatch()
                TCB::timeSliceCounter = 0;
                __asm__ volatile ("addi sp, sp, -16");
                __asm__ volatile ("sd %[sepc], 0 (sp)" : : [sepc] "r"(sepc));
                __asm__ volatile ("sd %[sstatus], 8 (sp)" : : [sstatus] "r"(sstatus));
                TCB::dispatch();
                __asm__ volatile ("ld %[sepc], 0 (sp)" : [sepc] "=r"(sepc));
                __asm__ volatile ("ld %[sstatus], 8 (sp)" : [sstatus] "=r"(sstatus));
                __asm__ volatile ("addi sp, sp, 16");
            }
            break;
            case 0x11UL: {
                //int thread_create ( thread_t* handle, void(*start_routine)(void*), void* stack )
                uint64 handle, routine, args, stackSpace;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (handle) : [sp] "r" (oldSP)); //x11 = a1
                __asm__ volatile ("ld %[a], 12 * 8 ( %[sp])" : [a] "=r" (routine) : [sp] "r" (oldSP)); //x12 = a2
                __asm__ volatile ("ld %[a], 13 * 8 ( %[sp])" : [a] "=r" (args) : [sp] "r" (oldSP)); //x13 = a3
                __asm__ volatile ("ld %[a], 14 * 8 ( %[sp])" : [a] "=r" (stackSpace) : [sp] "r" (oldSP)); //x14 = a4
                TCB* thread = TCB::createThread((TCB::Body) routine, (void*) args, (void*) stackSpace);
                args = (uint64) thread;
                __asm__ volatile ("sd %[val], 0 ( %[pointer])" : : [val] "r" (args), [pointer] "r" (handle));
                uint64 tmp = 0;
                __asm__ volatile ("sd %[a], 10 * 8 ( %[sp])" :  : [a] "r" (tmp) , [sp] "r" (oldSP)); //x10 = a0
            }
            break;
            case 0x01UL: {
                //void* mem_alloc (size_t size)
                size_t a1;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (a1) : [sp] "r" (oldSP)); //x11 = a1
                uint64 addr = MemoryAllocator::allocate(a1);
                __asm__ volatile ("sd %[a], 10 * 8 ( %[sp])" : : [a] "r" (addr) , [sp] "r" (oldSP)); //x10 = a0
            }
                break;
            case 0x02UL: {
                //int mem_free (void*)
                uint64 a1;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (a1) : [sp] "r" (oldSP)); //x11 = a1
                uint64 ret = MemoryAllocator::free((void *) a1);
                __asm__ volatile ("sd %[a], 10 * 8 ( %[sp])" : : [a] "r" (ret) , [sp] "r" (oldSP)); //x10 = a0
            }
                break;
            case 0x12UL:{
                //int thread_exit ()
                TCB::running->setFinished(true);
                TCB::dispatch();
            }
            break;
            case 0x21UL:{
                //int sem_open ( sem_t* handle, unsigned initValue)
                uint64 handle, value;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (handle) : [sp] "r" (oldSP)); //x11 = a1
                __asm__ volatile ("ld %[a], 12 * 8 ( %[sp])" : [a] "=r" (value) : [sp] "r" (oldSP)); //x12 = a2
                Sem* sem = (Sem*)MemoryAllocator::allocate(sizeof(Sem));
                sem->value = value;
                sem->waitingQueue.head= nullptr;
                sem->waitingQueue.tail= nullptr;
                value = (uint64) sem;
                __asm__ volatile ("sd %[val], 0 ( %[pointer])" : : [val] "r" (value), [pointer] "r" (handle));
                uint64 tmp=0;
                __asm__ volatile ("mv a0, %[data]" : : [data] "r" (handle));
                __asm__ volatile ("sd %[a], 10 * 8 ( %[sp])" : : [a] "r" (tmp) , [sp] "r" (oldSP)); //x10 = a0
            }
            break;
            case 0x23UL:{
                //int sem_wait (sem_t id)
                uint64 sem;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (sem) : [sp] "r" (oldSP)); //x11 = a1
                Sem* semaphore = (Sem*) sem;

                __asm__ volatile ("addi sp, sp, -16");
                __asm__ volatile ("sd %[sepc], 0 (sp)" : : [sepc] "r"(sepc));
                __asm__ volatile ("sd %[sstatus], 8 (sp)" : : [sstatus] "r"(sstatus));

                semaphore->wait();

                __asm__ volatile ("ld %[sepc], 0 (sp)" : [sepc] "=r"(sepc));
                __asm__ volatile ("ld %[sstatus], 8 (sp)" : [sstatus] "=r"(sstatus));
                __asm__ volatile ("addi sp, sp, 16");
            }
                break;
            case 0x24UL:{
                uint64 sem;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (sem) : [sp] "r" (oldSP)); //x11 = a1
                Sem* semaphore = (Sem*) sem;

                semaphore->signal();

            }
                break;
            case 0x22UL:{
                //int sem_close (sem_t handle)
                uint64 sem;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (sem) : [sp] "r" (oldSP)); //x11 = a1
                Sem* semaphore = (Sem*) sem;

                semaphore->exitSemaphore();
                MemoryAllocator::free(semaphore);
            }
                break;
            case 0x31UL:{
                //int time_sleep (time_t)
                uint64 time;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (time) : [sp] "r" (oldSP)); //x11 = a1

                __asm__ volatile ("addi sp, sp, -16");
                __asm__ volatile ("sd %[sepc], 0 (sp)" : : [sepc] "r"(sepc));
                __asm__ volatile ("sd %[sstatus], 8 (sp)" : : [sstatus] "r"(sstatus));
                TCB::sleep(time);
                __asm__ volatile ("ld %[sepc], 0 (sp)" : [sepc] "=r"(sepc));
                __asm__ volatile ("ld %[sstatus], 8 (sp)" : [sstatus] "=r"(sstatus));
                __asm__ volatile ("addi sp, sp, 16");
            }
                break;
            case 0x41UL:{
                //char getc()

                __asm__ volatile ("addi sp, sp, -16");
                __asm__ volatile ("sd %[sepc], 0 (sp)" : : [sepc] "r"(sepc));
                __asm__ volatile ("sd %[sstatus], 8 (sp)" : : [sstatus] "r"(sstatus));
                uint64 c = (uint64) KConsole::__getc();
                __asm__ volatile ("sd %[a], 10 * 8 ( %[sp])" : : [a] "r" (c) , [sp] "r" (oldSP)); //x10 = a0
                __asm__ volatile ("ld %[sepc], 0 (sp)" : [sepc] "=r"(sepc));
                __asm__ volatile ("ld %[sstatus], 8 (sp)" : [sstatus] "=r"(sstatus));
                __asm__ volatile ("addi sp, sp, 16");
            }
                break;
            case 0x42UL:{
                //void putc(char c)
                uint64 c;
                __asm__ volatile ("ld %[a], 11 * 8 ( %[sp])" : [a] "=r" (c) : [sp] "r" (oldSP)); //x11 = a1

                __asm__ volatile ("addi sp, sp, -16");
                __asm__ volatile ("sd %[sepc], 0 (sp)" : : [sepc] "r"(sepc));
                __asm__ volatile ("sd %[sstatus], 8 (sp)" : : [sstatus] "r"(sstatus));
                KConsole::__putc(c);
                __asm__ volatile ("ld %[sepc], 0 (sp)" : [sepc] "=r"(sepc));
                __asm__ volatile ("ld %[sstatus], 8 (sp)" : [sstatus] "=r"(sstatus));
                __asm__ volatile ("addi sp, sp, 16");
            }
                break;
        }
        w_sepc(sepc);
        w_sstatus(sstatus);
    }else if(scause == (0x01UL<<63 | 1)){
        //supervisor software interrupt (timer)
        TCB::timeSliceCounter++;
        TCB::tick();
        if(TCB::timeSliceCounter >= TCB::running->timeSlice){
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            __asm__ volatile ("addi sp, sp, -16");
            __asm__ volatile ("sd %[sepc], 0 (sp)" : : [sepc] "r" (sepc));
            __asm__ volatile ("sd %[sstatus], 8 (sp)" : : [sstatus] "r" (sstatus));
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            __asm__ volatile ("ld %[sepc], 0 (sp)" : [sepc] "=r" (sepc));
            __asm__ volatile ("ld %[sstatus], 8 (sp)" : [sstatus] "=r" (sstatus));
            __asm__ volatile ("addi sp, sp, 16");
            w_sepc(sepc);
            w_sstatus(sstatus);
        }
        mc_sip(2);  //SSIP clear (software interrupt pending)
    }else if(scause == (0x01UL<<63 | 9)){
        //supervisor external interrupt
        //console sent a character and raised interrupt
        uint64 sepc = r_sepc();
        uint64 sstatus = r_sstatus();
        __asm__ volatile ("addi sp, sp, -16");
        __asm__ volatile ("sd %[sepc], 0 (sp)" : : [sepc] "r" (sepc));
        __asm__ volatile ("sd %[sstatus], 8 (sp)" : : [sstatus] "r" (sstatus));
        KConsole::console_handler();
        __asm__ volatile ("ld %[sepc], 0 (sp)" : [sepc] "=r" (sepc));
        __asm__ volatile ("ld %[sstatus], 8 (sp)" : [sstatus] "=r" (sstatus));
        __asm__ volatile ("addi sp, sp, 16");
        w_sepc(sepc);
        w_sstatus(sstatus);
    }else{
        //unexpected interrupt cause
    }
    __asm__ volatile("addi sp, sp, 16");
    if(TCB::running->priority)
        ms_sstatus(256);        //SPP
    else
        mc_sstatus(256);
}

void Riscv::popFromStatusToCpu() {
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile ("sret");
}

