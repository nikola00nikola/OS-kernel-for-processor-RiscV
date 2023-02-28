
#ifndef PROJECT_BASE_V1_0_SYSCALL_C_H
#define PROJECT_BASE_V1_0_SYSCALL_C_H
#include "tcb.hpp"
#include "semaphore.hpp"
const int EOF = -1;

void* mem_alloc (size_t size);
//allocates size bytes of data from heap (Rounded to MEM_BLOCK_SIZE bytes)

int mem_free (void* p);
//deallocates memory from location p, return 0 if success else negative value

int thread_create ( thread_t* handle, void(*start_routine)(void*), void* arg );
//creates thread over function start_routine with argument arg, and stores it in handle

void thread_dispatch ();
//gives proccessor to other thread (if there is other thread ready for running)

int thread_exit ();
//kills running thread

int sem_open ( sem_t* handle, unsigned initVal);
//creates semaphore with value initVal, and stores it in handle

int sem_wait (sem_t sem);
//thread waits on semaphore sem

int sem_signal (sem_t sem);
//signal semaphore sem

int sem_close (sem_t sem);
//close semaphore sem

typedef uint64 time_t;

int time_sleep (time_t time);
//running thread sleeps time * 0.1 second

char getc();
//gets character from input buffer

void putc(char);
//put character in output buffer

#endif //PROJECT_BASE_V1_0_SYSCALL_C_H
