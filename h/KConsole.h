#ifndef _KConsole_h_
#define _KConsole_h_
#include "consoleBuffer.h"


class KConsole{
public:
    static void __putc(char chr);

    static char __getc();

    static void console_handler();
    static void workerKernel(void*);
    static void init();

    static ConsoleBuffer* outBuffer;
    static ConsoleBuffer* inBuffer;
    static Sem* sem1;
    static Sem* sem2;

    static void putCharToInBuffer(char c);

};

#endif