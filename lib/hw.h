#pragma once

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;

typedef uint64 size_t;
typedef uint64 time_t;

static const size_t DEFAULT_STACK_SIZE = 4096;
static const size_t DEFAULT_TIME_SLICE = 2;

static const size_t MEM_BLOCK_SIZE = 64;    //min block size for allocating memory from heap

static const uint64 CONSOLE_IRQ = 10;   //id of external interrupt for console
static const uint64 CONSOLE_TX_STATUS_BIT = 1 << 5;     //ready bit for send data to console( write)
static const uint64 CONSOLE_RX_STATUS_BIT = 1;          //ready bit for recieve data from console( read)

//------------------------------------------------------------------------------------------------------------
//ONLY PRE-CONFIGURED:


extern const uint64 CONSOLE_STATUS;     //address of status register, contains ready bits
extern const uint64 CONSOLE_TX_DATA;    //address of temporary TX register, 
                                        //controller passess data from TX register to console output

extern const uint64 CONSOLE_RX_DATA;    //address of temporary RX register, contains data recieved from keyboard

extern const void* HEAP_START_ADDR;
extern const void* HEAP_END_ADDR;

#ifdef __cplusplus
extern "C" {
#endif

    int plic_claim(void);   //get id of device that generated external interrupt from controller

    void plic_complete(int irq);    //inform controller that interrupt is processed

#ifdef __cplusplus
}
#endif
