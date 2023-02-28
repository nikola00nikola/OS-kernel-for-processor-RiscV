#ifndef _OS1_riscv_hpp_
#define _OS1_riscv_hpp_
#include "../lib/hw.h"

class Riscv{
public:
    static void pushRegisters();

    static void popRegisters();

    static uint64 r_scause();
    static uint64 r_sstatus();
    static uint64 r_sepc();

    static void w_scause(uint64 scause);
    static void w_sstatus(uint64 sstatus);
    static void w_sepc(uint64 sepc);
    static void w_stvec(uint64 stvec);

    static uint8 rB_mem(uint64 addr);

    static void mc_sip(uint64 mask);
    static void ms_sstatus(uint64 mask);
    static void mc_sstatus(uint64 mask);

    static void popFromStatusToCpu();   //non inline method

    //interrupt service routine
    static void supervisorTrap();
private:
    static void handleSupervisorTrap();
};

inline uint64 Riscv::r_scause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause": [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause) {
    __asm__ volatile ("csrw scause, %[scause]": : [scause]"r"(scause));
}

inline uint8 Riscv::rB_mem(uint64 addr) {
    uint8 volatile tmp;
    __asm__ volatile ("lb %[tmp], 0(%[addr])" : [tmp] "=r" (tmp) : [addr] "r" (addr));
    return tmp;
}

inline void Riscv::mc_sip(uint64 mask) {
    __asm__ volatile ("csrc sip, %[mask]": : [mask] "r" (mask));
}

inline uint64 Riscv::r_sepc(){
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r" (sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r" (sepc));
}

inline uint64 Riscv::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus": [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]": : [sstatus]"r"(sstatus));
}

inline void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r" (stvec));
}

inline void Riscv::ms_sstatus(uint64 mask) {
    __asm__ volatile ("csrs sstatus, %[mask]": : [mask] "r" (mask));
}

inline void Riscv::mc_sstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[mask]": : [mask] "r" (mask));
}

#endif