#ifndef SYSCALL_H
#define SYSCALL_H
/*
    Interface for all the kernel end sys call handlers

    Formal description of how syscalls are handled, hopefully this will also help with interrupts in the future

    User stack state goes like this:

    Arg 0           <- SP currently points here
    ...
    Arg N
    CPSR            <- !!!! This where the stackEntry in the task descriptor should point to
    R0-15           <- Stored SP state points here when restored

    Individual handlers are dispatched by the sys_handler, and will know the size of N since function signatures are fixed.
    With enough effort, you can implement variadic arguments on the sys calls, but it would be quite difficult to wind and unwind

    Since individual handler functions knows the number of arguments, we can safely unwind back to the CPSR point
    At this point, the handlers functions are responsible for decrementing the stack pointer so that all the arguments received are popped off the stack
    This will allow syscal handler to update the stackEntry field in the current task to be the position of the CPSR
    This order is important as stackEntry is used by the exitKernel to use as the beginning point of the function
*/

//The generic handler
//Redirects to the approrpiate handler based on the interrupt code
void sys_handler();

//Eventually
//void interrupt_handler();

void sysYield();

void sysGetTid();

void sysGetPid();
//no arguments because we will be manually retrieving it from the user stack
void sysCreateTask();

void sysExit();

void sysDestroy();

void sysSend();

void sysReceive();

void sysReply();

void setUpSWIHandler(void* handle_swi);

#endif
