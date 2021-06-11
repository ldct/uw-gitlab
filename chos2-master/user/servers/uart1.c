#include <bwio.h>
#include <ts7200.h>
#include <sendReceiveReply.h>
#include <interrupt.h>
#include <nameServer.h>
#include <clockServer.h>
#include <queue.h>
#include <uart1.h>
#include <io.h>
#include <ipc_codes.h>
#include <syslib.h>
#include <panic.h>

#define MSG_MAX 24

void com1TxServer() {
    RegisterAs("com1TxS");

    char requestBuf[MSG_MAX];
    int caller;

    int notifierTid = -1;
    int shuttingDown = 0;
    int shutDownCallerTid = -1;

    Queue com1OutputQueue;
    initializeQueue("com1Output", &com1OutputQueue);

    Queue com1CallerQueue;
    initializeQueue("com1CallerQueue", &com1CallerQueue);

    int txNotifierReady = 0; // The FIFO is empty
    int msien_flips = 2; // the first time we enter the loop, we are ok to send
    int awaiting_sensor_report = 0;

    while (1) {
        Receive(&caller, requestBuf, MSG_MAX);
        if (requestBuf[0] == UART1_SENSOR_REPORT) {
            awaiting_sensor_report = 0;
            if (!shuttingDown) {
                Reply0(caller);
            } else {
                Reply0(shutDownCallerTid);
            }
        } else if (requestBuf[0] == UART1_MSIEN_FLIP) {
            msien_flips++;
            Reply0(caller);
        } else if (requestBuf[0] == UART1_SHUT_DOWN) {
            shuttingDown = 1;
            shutDownCallerTid = caller;
        } else if (requestBuf[0] == UART1_TX_READY) {
            txNotifierReady = 1;
            notifierTid = caller;
        } else if (requestBuf[0] == 'N'/*onblocking send*/) {
            push(&com1OutputQueue, (void*)requestBuf[1]);
            push(&com1CallerQueue, (void*)(-1));
            Reply0(caller);
        } else if (requestBuf[0] == 'S'/*end*/) {
            push(&com1OutputQueue, (void*)requestBuf[1]);
            push(&com1CallerQueue, (void*)caller);
        } else if (requestBuf[0] == UART1_ATOMIC_SEND) {
            push(&com1OutputQueue, (void*)requestBuf[1]);
            push(&com1OutputQueue, (void*)requestBuf[2]);
            push(&com1CallerQueue, (void*)(-1));
            push(&com1CallerQueue, (void*)caller);
        } else if (requestBuf[0] == UART1_ATOMIC_NB_SEND) {
            push(&com1OutputQueue, (void*)requestBuf[1]);
            push(&com1OutputQueue, (void*)requestBuf[2]);
            push(&com1CallerQueue, (void*)(-1));
            push(&com1CallerQueue, (void*)(-1));
            Reply0(caller);
        } else {
            PANIC("unknown IPC message 1");
        }

        if (!awaiting_sensor_report && txNotifierReady && msien_flips >= 2 && ringFill(&com1OutputQueue)) {
            char c = (char) pop(&com1OutputQueue);
            int caller = (int) pop(&com1CallerQueue);
            if (c == 0x85) {
                awaiting_sensor_report = 1;
            }
            ASSERT(notifierTid > 0, "unknown notifier");
            Reply(notifierTid, &c, 1);
            if (caller != -1) {
                Reply0(caller);
            }
            txNotifierReady = 0;
            msien_flips = 0;
        }
    }
}

void com1TxNotifier() {
    RegisterAs("com1TxN");

    int serverTid = WhoIs("com1TxS");

    int volatile * flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART1_BASE + UART_DATA_OFFSET );

    while (1) {
        AwaitEvent(INT_UART1_TX);
        char c = c_Send_c(serverTid, UART1_TX_READY);
        if (*flags & TXFF_MASK) {
            PANIC("Return from UART interrupt but TX FIFO is full");
        }
        // todo: status line?
        // if (c == 0x85) {
        //     lprintf(COM2, "_");
        // } else {
        //     lprintf(COM2, "<%x>", c);
        // }
        *data = c;
    }

}

void com1RxNotifier() {
    int volatile * flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART1_BASE + UART_DATA_OFFSET );

    for (;;) {
        /*

        Await(...)
        Send(...)

        If the Send takes too long, kernel will panic
        */
        AwaitEvent(INT_UART1_RX);
        while (!(*flags & RXFE_MASK)) {
            char sendReq[2];

            sendReq[0] = UART1_RX;
            sendReq[1] = *data;
            Send(WhoIs("com1RxS"), sendReq, 2, 0x0, 0);
        }

    }
}

void com1MsNotifier() {
    int serverTid = WhoIs("com1TxS");

    for (;;) {
        AwaitEvent(INT_UART1_MS);
        v_Send_c(serverTid, UART1_MSIEN_FLIP);
    }
}

void com1RxServer() {
    RegisterAs("com1RxS");

    char requestBuf[MSG_MAX];
    int caller;

    Queue com1InputQueue;
    initializeQueue("com1InputQueue", &com1InputQueue);

    int getcCaller = -1;

    while (1) {
        Receive(&caller, requestBuf, MSG_MAX);
        if (requestBuf[0] == UART1_RX) {
            Reply0(caller);
            push(&com1InputQueue, (void*)requestBuf[1]);
        } else if (requestBuf[0] == UART_GETC) {
            getcCaller = caller;
        } else {
            PANIC("unknown IPC message 2");
        }

        if (getcCaller != -1 && ringFill(&com1InputQueue)) {
            char c = (char) (int) pop(&com1InputQueue);
            Reply(getcCaller, &c, 1);
            getcCaller = -1;
        }

    }
}
