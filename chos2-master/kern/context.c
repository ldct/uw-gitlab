#include <ARM.h>
#include <kern.h>
#include <bwio.h>

void exitKernel(void* processStackPtr){
    // save kernel registers on kernel stack
    asm("STMFD SP!, {R0-R12, R14-R15}");
    asm("ADD R2, PC, #44");
    //sets where the return address should be
    asm("STR R2, [SP, #56]");

    asm("LDR R1, [R0]");
    asm("MSR SPSR, R1");

    //restores the stack pointer, minus the cpsr
    // sp <- r0 - 4
    asm("MSR CPSR_c, #0x9F");
    asm("ADD SP, R0, #4");
    
    asm("MSR CPSR_c, #0x93"); 
    asm("LDR R14, [R0, #60]");
    asm("MSR CPSR_c, #0x9F");
    //loads user mode registers
    //includes the PC register, and starts executing
    asm("LDMFD SP!, {R0-R12, R14}");
    asm("ADD SP, SP, #4");
    asm("MSR CPSR_c, #0x93");
    asm("MOVS R15, R14");
    // bwprintf(COM2, "end of exitKernel\r\n");
}

void __attribute__((naked)) enterKernel(){
    // bwprintf(COM2, "entering Kernel\r\n");
    asm("LDMFD SP!, {R0-R12, R14-R15}");
}
