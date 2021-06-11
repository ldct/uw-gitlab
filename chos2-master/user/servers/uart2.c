#include <bwio.h>
#include <ts7200.h>
#include <sendReceiveReply.h>
#include <interrupt.h>
#include <nameServer.h>
#include <clockServer.h>
#include <queue.h>
#include <uart2.h>
#include <io.h>
#include <panic.h>
#include <syslib.h>
#include <ipc_codes.h>

#define MSG_MAX 24

void com2TxServer() {
    RegisterAs("com2TxS");

    char requestBuf[MSG_MAX];
    int caller;

    int notifierTid;

    Queue com1Queue;
    initializeQueue("com1Output", &com1Queue);

    int txNotifierReady = 0;

    while (1) {
        Receive(&caller, requestBuf, MSG_MAX);
        if (requestBuf[0] == UART2_TX_READY) {
            txNotifierReady = 1;
            notifierTid = caller;
        } else if (requestBuf[0] == 'S'/*end, nonblocking*/) {
            push(&com1Queue, (void*)requestBuf[1]);
            Reply0(caller);
        } else {
            PANIC("Unknown IPC message 3");
        }
        if (txNotifierReady && ringFill(&com1Queue)) {
            char c = (char) (int) pop(&com1Queue);
            Reply(notifierTid, &c, 1);
            txNotifierReady = 0;
        }
    }
}

void com2TxNotifier() {
    RegisterAs("com2TxN");

    int serverTid = WhoIs("com2TxS");

    int volatile * flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART2_BASE + UART_DATA_OFFSET );

    while (1) {
        AwaitEvent(INT_UART2_TX);
        char c = c_Send_c(serverTid, UART2_TX_READY);
        if (*flags & TXFF_MASK) {
            setEnabledDevices(0x0, 0x0);
            bwprintf(COM2, "PANIC: Return from UART interrupt but TX FIFO is full\r\n");
            for (;;) {}
        }
        *data = c;
    }

}

void com2RxServer() {
    RegisterAs("com2RxS");

    char requestBuf[MSG_MAX];
    int caller;

    Queue com2InputQueue;
    initializeQueue("com2Input", &com2InputQueue);

    int getcCaller = -1;

    while (1) {
        Receive(&caller, requestBuf, MSG_MAX);
        if (requestBuf[0] == UART2_RX) {
            Reply0(caller);
            push(&com2InputQueue, (void*)requestBuf[1]);
        } else if (requestBuf[0] == UART_GETC) {
            getcCaller = caller;
        } else {
            PANIC("unknown IPC message 4");
        }

        if (getcCaller != -1 && ringFill(&com2InputQueue)) {
            char c = (char) (int) pop(&com2InputQueue);
            Reply(getcCaller, &c, 1);
            getcCaller = -1;
        }
    }
}

void com2RxNotifier() {
    int volatile * flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART2_BASE + UART_DATA_OFFSET );

    for (;;) {
        AwaitEvent(INT_UART2_RX);
        while (!(*flags & RXFE_MASK)) {
            char sendReq[2];

            sendReq[0] = UART2_RX;
            sendReq[1] = *data;
            Send(WhoIs("com2RxS"), sendReq, 2, 0x0, 0);
        }
    }
}
