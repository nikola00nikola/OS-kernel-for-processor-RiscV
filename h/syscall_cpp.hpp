#ifndef _syscall_cpp
#define _syscall_cpp
#include "syscall_c.h"

//Java-like thread api
class Thread {
public:
    virtual ~Thread ();
    int start ();   
    static void dispatch ();
    static int sleep (time_t);
    virtual void run() {

    }
    Thread(void (*body)(void*), void* arg); //alternate way of starting thread, instead of calling start()
protected:
    Thread ();
private:
    thread_t myHandle;
    static void runWraper(void* p);
};


class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};


class PeriodicThread : public Thread {      //perodically running periodicActivation()
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation() {}

    static void periodicBody(void* p);
    static List<PeriodicThread> listaPT;
    static List<uint64> periods;
};


class Console {
public:
    static char getc ();
    static void putc (char);
};




#endif