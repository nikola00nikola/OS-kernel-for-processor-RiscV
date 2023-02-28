#ifndef _OS1_scheduler_hpp_
#define _OS1_scheduler_hpp_

#include "../h/list.hpp"

class TCB;

class Scheduler{
public:
    static List<TCB> readyCoroutineQueue;
public:
    static TCB* get();
    static void put(TCB* ccb);
};


#endif