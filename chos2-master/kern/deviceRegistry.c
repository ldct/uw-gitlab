#include <deviceRegistry.h>
#include <interrupt.h>
#include <bwio.h>
#include <panic.h>
#include <ts7200.h>

extern Scheduler* scheduler;

void initializeDeviceRegistry(DeviceRegistry* registry) {
    initializeMap(&(registry->deviceMap));
}

/*
The following devices can be passed to WaitForInterrupt:
- INT_TC1UI
- INT_UART1_TX
- INT_UART1_RX
- INT_UART1_MS
- INT_UART2_TX
- INT_UART2_RX

The following cannot:
- INT_UART1_COMBINED
*/
void WaitForInterrupt(DeviceRegistry* registry, Task* task, int device) {

    int* uart1_ctrl = (int *)( UART1_BASE + UART_CTLR_OFFSET );

    if (device < 32) {
        enableDevice(1 << device, 0x0);
    } else if (device < 100) {
        PANIC("cannot await for devices 32-64 directly");
    } else /* device >= 100; synthetic interrupt */ {
        if (device == INT_UART1_TX) {
            *uart1_ctrl |= TIEN_MASK;
        } else if (device == INT_UART1_RX) {
            *uart1_ctrl |= RIEN_MASK;
        } else if (device == INT_UART1_MS) {
            *uart1_ctrl |= MSIEN_MASK;
        } else {
            PANIC("unknown synthetic device");
        }
    }
    ASSERT(
        1 == putMap(&(registry->deviceMap), device, task),
        "double waiting");;

}

void WakeForInterrupt(DeviceRegistry* registry, int device) {
    Task* task = removeMap(&(registry->deviceMap), device);
    if (device != INT_UART1_MS && device != INT_TC1UI && !task) {
        /*
        If an interrupt fires and no one is installed as a handler, this is
        in most cases an error as all user code can be written in a way so that
        a handler is installed before an action is taken that will cause the
        interrupt to fire. The exception is INT_UART1_MS on the non-train
        terminals.
        */
        setEnabledDevices(0x0, 0x0);
        bwprintf(COM2, "PANIC: no one waiting for %d\r\n", device);
    }
    if (task) {
        insertTaskToQueue(scheduler, task);
    }
}
