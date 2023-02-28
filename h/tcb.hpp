#ifndef _OS1_ccb_hpp_
#define _OS1_ccb_hpp_
#include "../lib/hw.h"
#include "../h/scheduler.hpp"
#include "../h/riscv.hpp"
#include "../h/semaphore.hpp"
typedef TCB* thread_t;

//THREAD CONTROL BLOCK
class TCB{
public:
    ~TCB(){
        MemoryAllocator::free(stack);
    }

    using Body = void(*)(void*);
    static TCB* createThread(Body body, void* args, void* stack, bool putToSch= true, bool priority = false);
    bool isFinished() const{return finished; }

    void setFinished(bool finished){ TCB::finished=finished; }

    uint64 getTimeSLice() const{
        return timeSlice;
    }

    bool priority;
    static void yield();
    static TCB* bootloop;
    static void init();
    static TCB* running;
    static uint64 constexpr STACK_SIZE = 1024;
    static void sleep(uint64 sleepTime);

    struct THEADER{
        TCB* thread;
        uint64 sleepTime;
    };

    static void tick();
    static void funcBootLoop(void* );
private:
   /* TCB(Body body, void* args, void* stackSpace, uint64 timeSlice = TIME_SLICE) : body(body), arg(args), stack((uint64*) stackSpace),
                     context({
                         (uint64) &threadWraper,
                        stack!=nullptr ? (uint64)&stack[STACK_SIZE] : 0
        }),
                     timeSlice(timeSlice),
                     finished(false){
        if(body != nullptr)
            Scheduler::put(this);
    }*/

    struct Context{
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* arg;
    uint64* stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    friend class Riscv;
    friend class Sem;
    static void contextSwitch(Context* oldContext, Context* runningContext);
    static void putToSleep(uint64 time);
    static void dispatch(bool dontContinueOld=false);
    static void threadWraper();
    static List<THEADER> sleepingThreads;
    static uint64 constexpr TIME_SLICE = 2;
    static uint64 timeSliceCounter;
};

#endif