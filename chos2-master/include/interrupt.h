#ifndef INTERRUPT_H
#define INTERRUPT_H



//Refer to the section "6.2: Registers" in the EP93xx user guide page 170
#define VIC1ADDR 0x800B0000
#define VIC2ADDR 0x800C0000
#define VIC_RAW_INTR 0x08 // VICxRawIntr
#define VIC_ENABLE 0x10 // VICxIntEnable
#define VIC_ENABLE_CLEAR 0x14 // VICxIntEnClear

#define INT_TC1UI      4
#define INT_UART2_RX   25
#define INT_UART2_TX   26
#define INT_UART1_COMBINED 52

// synthetic interrupt

#define INT_UART1_TX    101
#define INT_UART1_RX    102
#define INT_UART1_MS    103


void interruptHandler();

void installInterruptHandler(void* handler);

void setEnabledDevices(unsigned deviceList1, unsigned deviceList2);

void enableDevice(unsigned deviceList1, unsigned deviceList2);

void disableDevice(unsigned deviceList1, unsigned deviceList2);

#endif
