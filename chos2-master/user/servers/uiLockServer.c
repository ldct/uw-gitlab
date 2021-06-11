#include <clockServer.h>
#include <nameServer.h>
#include <ipc_codes.h>
#include <queue.h>
#include <syslib.h>
#include <panic.h>

int uiLocked;

void lockServer() {

    RegisterAs("lockS");

    int whoHasTheLock = -1;

    char requestBuf;
    int caller;

    Queue whoIsWaiting;
    initializeQueue("lsWho", &whoIsWaiting);

    for (;;) {
        Receive(&caller, &requestBuf, 1);
        if (requestBuf == LOCKSERVER_ACQUIRE) {
            push(&whoIsWaiting, (void*) caller);
        } else if (requestBuf == LOCKSERVER_RELEASE) {
            Reply0(caller);
            whoHasTheLock = -1;
        } else {
            PANIC("unknown IPC message");
        }
        if (whoHasTheLock == -1 && ringFill(&whoIsWaiting) > 0) {
            whoHasTheLock = (int) pop(&whoIsWaiting);
            Reply0(whoHasTheLock);
        }
    }
}

void acquireUiLock() {
    v_Send_c(WhoIs("lockS"), LOCKSERVER_ACQUIRE);
}

void releaseUiLock() {
    v_Send_c(WhoIs("lockS"), LOCKSERVER_RELEASE);
}
