#include "../h/syscall_c.h"
#include "../h/MemoryAllocator.h"

void *mem_alloc(size_t size) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    uint64 tmp = 0x1UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));

    __asm__ volatile("addi sp, sp, 16");
    return (void*) tmp;
}

int mem_free(void * pointer) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    uint64 tmp = 0x2UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));

    __asm__ volatile("addi sp, sp, 16");
    return (int) tmp;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    __asm__ volatile("addi sp, sp, -32");
    __asm__ volatile("sd a0, 0 (sp)");
    __asm__ volatile("sd a1, 8 (sp)");
    __asm__ volatile("sd a2, 16 (sp)");
    uint64 tmp = MemoryAllocator::allocate(sizeof(uint64) * TCB::STACK_SIZE);
    if(tmp == 0)
        return -1;
    __asm__ volatile("sd %[tmp], 24 (sp)" : : [tmp] "r" (tmp));
    tmp = 0x11UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ld a2, 8 (sp)");
    __asm__ volatile("ld a3, 16 (sp)");
    __asm__ volatile("ld a4, 24 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));

    __asm__ volatile("addi sp, sp, 32");
    return (int) tmp;
}

void thread_dispatch() {
    uint64 tmp = 0x13UL;
    __asm__ volatile ("mv a0, %[tmp]" : : [tmp] "r" (tmp));
    __asm__ volatile ("ecall");
}

int thread_exit() {
    uint64 tmp = 0x12UL;
    __asm__ volatile ("mv a0, %[tmp]" : : [tmp] "r" (tmp));
    __asm__ volatile ("ecall");
    return -1;
}

int sem_open(sem_t *handle, unsigned int init) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    __asm__ volatile("sd a1, 8 (sp)");
    uint64 tmp = 0x21UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ld a2, 8 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));

    __asm__ volatile("addi sp, sp, 16");
    return (int) tmp;
}

int sem_wait(sem_t id) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    uint64 tmp = 0x23UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));

    __asm__ volatile("addi sp, sp, 16");
    return (int) tmp;
}

int sem_signal(sem_t id) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    uint64 tmp = 0x24UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));

    __asm__ volatile("addi sp, sp, 16");
    return (int) tmp;
}

int sem_close(sem_t handle) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    uint64 tmp = 0x22UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));
    __asm__ volatile("addi sp, sp, 16");
    return (int) tmp;
}

int time_sleep(time_t time) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    uint64 tmp = 0x31UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));
    __asm__ volatile("addi sp, sp, 16");
    return 0;
}

char getc() {
    __asm__ volatile("addi sp, sp, -16");
    uint64 tmp = 0x41UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[x], a0" : [x] "=r" (tmp));
    __asm__ volatile("addi sp, sp, 16");
    return (char) tmp;
}

void putc(char c) {
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0 (sp)");
    uint64 tmp = 0x42UL;
    __asm__ volatile("mv a0, %[x]" : : [x] "r" (tmp));
    __asm__ volatile("ld a1, 0 (sp)");
    __asm__ volatile("ecall");
    __asm__ volatile("addi sp, sp, 16");
}
