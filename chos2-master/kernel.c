#include <ts7200.h>
#include <ARM.h>
#include <syscall.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <userprogram.h>
#include <nameServer.h>
#include <bwio.h>
#include <ssrTest.h>
#include <k1.h>
#include <k2.h>
#include <k3.h>
#include <k4.h>
#include <timer.h>
#include <maptest.h>
#include <interrupt.h>
#include <idle.h>
#include <deviceRegistry.h>
#include <dump.h>
#include <kern.h>
#include <hud.h>

Scheduler base_scheduler;
Scheduler* scheduler;
COMM* com;
DeviceRegistry* registry;
Task idler;

/*

Global seed used for the deterministic PRNG. Every task that needs a PRNG
derives a task-local seed via `seed = seedSeed + MyTid()`.

*/
const unsigned seedSeed = 0xdeadbeef;

/*
The nameserver TID is stored in global memory and read by any task calling the
nameserver API.
*/
int nsTid;
int kernelRunning;

void initGlobals();

int utilTimePct;

int main( int argc, char* argv[] ) {

    bwsetfifo(COM1, OFF);
    bwsetfifo(COM2, OFF);
    bwsetspeed(COM1, 2400);
    bwsetstopbits(COM1, ON);

    initGlobals();

    bwprintf(COM2, "\033[2J");

    bwprintf(COM2, "\033[%d;%dr", SCROLL_START, SCROLL_END);
    bwprintf(COM2, "\033[%d;1H", SCROLL_START);

    setUpSWIHandler((void*)sys_handler);
    installInterruptHandler((void*)interruptHandler);

    /*

    `*` means enabled

    INT_UART1_COMBINED  *
        TIEN            _
        RIEN            _
        MSIEN           _
    INT_UART_2_COMBINED _
        TIEN            *
        RIEN            *

    This way, WaitForInterrupt works properly; for INT_UART2_* maps to the ICU
    mask and INT_UART1_* maps to the UART2_CTRL bits.

    INT_UART_2_COMBINED is never used since the only UART2 interrupts we need,
    i.e. TI and RI, are directly connected to the ICU. Conversely the TI and
    RI interrupts for UART1 are provided by the ICU, but we do not use them.

    */
    int* uart2_ctrl = (int *)( UART2_BASE + UART_CTLR_OFFSET );
    *uart2_ctrl |= (TIEN_MASK | RIEN_MASK);

    setEnabledDevices(
        (1 << INT_TC1UI),
        (1 << (INT_UART1_COMBINED - 32))
    );

    hypeTrain();
    scheduler = &base_scheduler;

    COMM commRelay;
    com = &commRelay;

    DeviceRegistry deviceRegistry;
    registry = &deviceRegistry;

    initializeScheduler(scheduler);
    initializeCOMM(com);
    initializeDeviceRegistry(registry);

    initializeTimer(3, 508000, getWrap(3), 0);

    scheduleTask(scheduler, 0, 0, (void*)k4_main);

    initializeTask(&idler, -1, 0, -1, HALTED, (void*)idle);

    unsigned totalFull = 0;
    unsigned totalUtil = 0;
    while (1) {
        if (!kernelRunning) return 0;

        unsigned t0 = getValue(3);
        while (1) {
            if (-1 == runFirstAvailableTask(scheduler)) break;
        }

        if (!kernelRunning) return 0;

        unsigned t1 = getValue(3);
        scheduler->currentTask = &idler;
        exitKernel(idler.stackEntry);
        unsigned t2 = getValue(3);

        totalUtil += (t0 - t1);
        totalFull += (t0 - t2);

        if (totalFull > 508000) {
            utilTimePct = 1000*totalUtil/totalFull;
            totalUtil = 0;
            totalFull = 0;
        }
    }
}
