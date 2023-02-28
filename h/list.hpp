#ifndef _OS1_list_hpp_
#define _OS1_list_hpp_

#include "MemoryAllocator.h"
#include "riscv.hpp"

template<typename T>
class List{
public:
    struct Elem{
        T* data;
        Elem* next;

    };
    Elem *head=nullptr, *tail= nullptr;

    friend class Riscv;
    friend class TCB;
    void addFirst(T* data){
        Elem* elem = (Elem*) MemoryAllocator::allocate(sizeof(Elem));
        elem->data = data;
        elem->next = head;
        head=elem;
        if(tail == nullptr)
            tail=head;
    }

    void setEmpty(){
        head = nullptr;
        tail = nullptr;
    }

    void addLast(T* data){
        Elem* elem = (Elem*) MemoryAllocator::allocate(sizeof(Elem));
        elem->data = data;
        elem->next = nullptr;
        if(tail){
            tail->next = elem;
            tail = elem;
        }else
            head = tail = elem;
    }

    T* removeFirst(){
        if(!head) return nullptr;
        Elem* elem=head;
        head = head->next;
        if(!head) tail=nullptr;
        T* ret=elem->data;
        MemoryAllocator::free(elem);
        return ret;
    }

    T* peekFirst(){
        if(!head) return nullptr;
        return head->data;
    }

    T* removeLast(){
        if(!head) return nullptr;
        Elem *prev = nullptr;
        for(Elem* curr=head;curr && curr!= tail ; curr=curr->next){
            prev=curr;
        }
        Elem* elem=tail;
        if(prev)
            prev->next=nullptr;
        else
            head=nullptr;
        tail=prev;

        T* ret = elem->data;
        MemoryAllocator::free(elem);
        return ret;
    }

    T* peekLast(){
        if(!tail)
            return nullptr;
        return tail->data;
    }

    bool remove(T* info){
        Elem *prev = nullptr, *cur = head;

        while(cur != nullptr && cur->data != info){
            prev = cur;
            cur = cur->next;
        }
        if(cur == nullptr)
            return false;
        if(prev)
            prev->next = cur->next;
        else
            head = cur->next;
        if(cur->next == nullptr)
            tail = prev;
        MemoryAllocator::free(cur);
        return true;
    }

};

#endif