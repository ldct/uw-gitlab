#include <minHeap.h>
#include <clockServer.h>
#include <nameServer.h>
#include <syslib.h>
#include <bwio.h>
#include <interrupt.h>
#include <ipc_codes.h>
#include <panic.h>
#include <timer.h>

#define CLOCK_MAX 24

void clockServer() {
    int ticks = 0;
    MinHeap waitList;

    RegisterAs("cs");
    initializeMinHeap(&waitList);
    char requestBuf[CLOCK_MAX];
    char command;
    int caller;

    while (1) {
        Receive(&caller, requestBuf, CLOCK_MAX);
        if (requestBuf[0] == CS_TICK) {
            ticks++;
            Reply0(caller);
            KV* taskWait = peek(&waitList);

            while(taskWait && taskWait->key <= ticks){
                v_Reply_i((int)(taskWait->value), ticks);
                removeMinHeap(&waitList);
                taskWait = peek(&waitList);
            }
        } else if (requestBuf[0] == CS_TIME) {
            v_Reply_i(caller, ticks);
        } else {
            command = requestBuf[0];
            int requestTicks;
            if (command == CS_DELAY) {
                requestTicks = ticks + *(int*)(requestBuf+4);
            } else if (command == 'U'){
                requestTicks = *(int*)(requestBuf+4);
            } else {
                PANIC("unknown IPC");
            }
            ASSERT(requestTicks >= ticks, "negative deadline");
            insertMinHeap(&waitList, requestTicks, (void*)caller);
        }
    }
}

int DelayUntil(int csTid, int deadline){
    char buffer[12];
    char receiveBuffer[8];
    buffer[0] = 'U';
    *(int*)(buffer + 4) = deadline;
    buffer[9] = 0;
    int status = Send(csTid, buffer, 12, receiveBuffer, CLOCK_MAX);
    ASSERT(status != -1, "Send failed");
    return *(int*)receiveBuffer;
}

int Time(int csTid) {
    char sendBuf = CS_TIME;
    char receiveBuffer[8];
    int status = Send(csTid, &sendBuf, 1, receiveBuffer, 8);
    ASSERT(status != -1, "Send failed");
    return *(int*)receiveBuffer;
}

void Delay(int csTid, int ticks) {
    char sendBuf[12];
    char receiveBuf[8];
    sendBuf[0] = CS_DELAY;
    *(int*)(sendBuf + 4) = ticks;
    sendBuf[9] = 0;
    int status = Send(csTid, sendBuf, 12, receiveBuf, CLOCK_MAX);
    ASSERT(status != -1, "Send failed");
}

void clockNotifier() {
    initializeTimer(1, 2000, 20, 1); // 10ms

    int tId = 0;
    while(!tId) tId = WhoIs("cs");

    for (;;) {
        AwaitEvent(INT_TC1UI);
        v_Send_c(tId, CS_TICK);
    }
}
