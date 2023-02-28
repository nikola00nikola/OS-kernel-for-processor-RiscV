#include "../h/syscall_cpp.hpp"

void* operator new(uint64 n) {
    return mem_alloc(n);
}

void* operator new[](uint64 n) {
    return mem_alloc(n);
}

void operator delete(void* p) noexcept {
    mem_free(p);
}

void operator delete[](void* p) noexcept {
    mem_free(p);
}

int Thread::start() {
    return thread_create(&myHandle, Thread::runWraper, this);
}

Thread::Thread(void (*body)(void*), void* arg) {
    thread_create(&myHandle, body, arg);
}

void Thread::runWraper(void *p) {
    Thread* t = (Thread*) p;
    t->run();
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

Thread::Thread() {

}


Thread::~Thread() {

}


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

List<PeriodicThread> PeriodicThread::listaPT;
List<uint64> PeriodicThread::periods;

void PeriodicThread::periodicBody(void *p) {
    List<PeriodicThread>::Elem* curP = listaPT.head;
    List<uint64>::Elem* curT = periods.head;
    PeriodicThread* nit= (PeriodicThread*) p;
    while((uint64) curP->data !=  (uint64)nit){
        curP=curP->next;
        curT=curT->next;
    }
    uint64 period = *curT->data;
    while(true){
        nit->periodicActivation();
        time_sleep(period);
    }
}

PeriodicThread::PeriodicThread(time_t period) : Thread(periodicBody, this){
    listaPT.addLast(this);
    uint64* p=new uint64[1];    
    *p = period;
    periods.addLast(p);
}


