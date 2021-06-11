#include <ARM.h>
#include <syscode.h>
#include <ts7200.h>
#include <timer.h>
#include <dump.h>
#include <deviceRegistry.h>
#include <interrupt.h>
#include <bwio.h>
#include <panic.h>

extern DeviceRegistry* registry;

void interruptProcessor() {
    int statusMask1 = *((unsigned*)VIC1ADDR);
    int statusMask2 = *((unsigned*)VIC2ADDR);

    int interruptsProcessed = 0;

    int* uart1_ctrl = (int *)( UART1_BASE + UART_CTLR_OFFSET );

    if (statusMask1 & (1 << INT_TC1UI)) {
        disableDevice((1 << INT_TC1UI), 0x0);
        WakeForInterrupt(registry, INT_TC1UI);
        // clear the timer
        *(volatile unsigned*)(TIMER1_BASE + CLR_OFFSET) = 0;
        interruptsProcessed++;
    }

    if (statusMask2 & (1 << (INT_UART1_COMBINED - 32))) {
        unsigned flag = *(volatile unsigned*)(0x808C001C);
        if (flag & 0x1) {
            *(volatile unsigned*)(0x808C001C) &= ~0x1;
            WakeForInterrupt(registry, INT_UART1_MS);
        }
        if (flag & 0x2){
            *uart1_ctrl &= ~RIEN_MASK;
            WakeForInterrupt(registry, INT_UART1_RX);
        }
        if (flag & 0x4) {
            *uart1_ctrl &= ~TIEN_MASK;
            WakeForInterrupt(registry, INT_UART1_TX);
        }
        if (flag & 0x8) {
            PANIC("Unexpected synthetic interrupt: RT");
        }
        interruptsProcessed++;
    }
    if (statusMask1 & (1 << INT_UART2_RX)) {
        disableDevice((1 << INT_UART2_RX), 0x0);
        WakeForInterrupt(registry, INT_UART2_RX);
        interruptsProcessed++;
    }
    if (statusMask1 & (1 << INT_UART2_TX)) {
        disableDevice((1 << INT_UART2_TX), 0x0);
        WakeForInterrupt(registry, INT_UART2_TX);
        interruptsProcessed++;
    }
    if (0 == interruptsProcessed) {
        bwprintf(COM2, "PANIC: Unexpected Interrupt\r\n");
        bwprintf(COM2, "Triggered Interrupts:\t%x\t%x\r\n", statusMask1, statusMask2);
        for (;;) {}
    }

}

void __attribute__((naked)) interruptHandler() {

    //BLOCK 1:
    //saving context as system mode

    //save the LR - 4 first as this is our reentry
    asm("SUB SP, SP, #4");
    asm("SUB LR, LR, #4");
    asm("STR LR, [SP]");

    asm("MSR CPSR_c, #"TOSTRING(SYS_MODE));

    asm("STMFD SP!, {R0-R12, R14-R15}");

    asm("MSR CPSR_c, #"TOSTRING(IRQ_MODE));
    asm("MRS R2, SPSR");
    asm("MSR CPSR_c, #"TOSTRING(SYS_MODE));
    asm("STMFD SP!, {R2}");

    asm("MSR CPSR_c, #"TOSTRING(IRQ_MODE));
    //handles actual processing
    asm("BL interruptProcessor");

    //restores the corrected lr
    //primed for arguments
    asm("LDR R0, [SP]");
    asm("ADD SP, SP, #4");

    asm("MSR CPSR_c, #"TOSTRING(SVC_MODE));

    //handles restoration
    asm("BL handleSuspendedTasks");

    //returns
    asm("B enterKernel");
}

void installInterruptHandler(void* handler){
    /*
    Install Interrupt handler
    The swi instruction executes at address 0x18. The following two lines write
    this to the memory addresses:

    0x18        LDR pc, [pc, #0x18]
    0x0c        ?
    ...         ?
    0x38        <absolute address of handle_Interrupt>

    */
    *(unsigned*)0x18 = 0xe59ff018;
    *(unsigned*)0x38 = (unsigned)handler;
}


void setEnabledDevices(unsigned deviceList1, unsigned deviceList2){
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE_CLEAR) = ~0;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE_CLEAR) = ~0;
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE) = deviceList1;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE) = deviceList2;
}

void disableDevice(unsigned deviceList1, unsigned deviceList2) {
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE_CLEAR) = deviceList1;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE_CLEAR) = deviceList2;
}

void enableDevice(unsigned deviceList1, unsigned deviceList2) {
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE) |= deviceList1;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE) |= deviceList2;
}
