
# Risc V OS Kernel

Little functional OS for Risc V processor from scratch.

RiscV-64IMA(I- basic operations with integers, M- mul/div with integers, A-atomic operations)

Embedded-like system (1 physical thread, virtual memory is actual physical memory) 
### User api for implemented system calls
User api is given in files [h/syscall_c.h](https://github.com/nikola00nikola/OS-kernel-for-processor-RiscV/blob/main/h/syscall_c.h) and [h/syscall_cpp.hpp](https://github.com/nikola00nikola/OS-kernel-for-processor-RiscV/blob/main/h/syscall_cpp.hpp) .

### Interrupt service routine
Riscv::supervisorTrap() is interrupt service routine for all interrupts, method is implemented in [src/supervisorTrap.S](https://github.com/nikola00nikola/OS-kernel-for-processor-RiscV/blob/main/h/syscall_c.h). Causes of interrupt are:
- system call
- software interrupt, generated from timer (generates every 0.1s, pre-configured)
- external interrupt, generated from external controller
Basic hardware data is given in [lib/hw.h](https://github.com/nikola00nikola/OS-kernel-for-processor-RiscV/blob/main/lib/hw.h) .

Most important classes:
- TCB (Thread control block)
- Scheduler
- MemoryAllocator