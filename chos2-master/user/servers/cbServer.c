#include <map.h>
#include <syslib.h>
#include <stdlib.h>
#include <io.h>
#include <panic.h>
#include <ipc_codes.h>
#include <nameServer.h>
#include <cbServer.h>

void sendTo(int tid, int sensorId) {
    char c[2];
    c[0] = TRAIN_SERVER_SENSOR_CB;
    c[1] = (char) sensorId;

    Send(tid, c, 2, 0x0, 0);
}

void cbServer() {

    RegisterAs("cbS");

    int primaryCb = -1;
    int secondaryCb = -1;
    int thirdCb = -1;

    int caller;
    char requestBuf[2];

    while (1) {
        Receive(&caller, requestBuf, 2);
        if (requestBuf[0] == CBS_REGISTER_CB) {
            Reply0(caller);
            if (primaryCb == -1) {
                primaryCb = caller;
            } else if (secondaryCb == -1) {
                secondaryCb = caller;
            } else if (thirdCb == -1) {
                thirdCb = caller;
            } else {
                PANIC("too many cb registered");
            }
        } else if (requestBuf[0] == CBS_FIRE_CB) {
            Reply0(caller);

            int sent = 0;

            int sensorId = (int) requestBuf[1];
            if (primaryCb != -1) {
                sent = 1;
                sendTo(primaryCb, sensorId);
            }
            if (secondaryCb != -1) {
                sent = 1;
                sendTo(secondaryCb, sensorId);
            }
            if (thirdCb != -1) {
                sent = 1;
                sendTo(thirdCb, sensorId);
            }

            if (sent == 0) {
                log("warning: no callbacks");
            }

        } else if (requestBuf[0] == CBS_DEREGISTER) {
            Reply0(caller);
            if (primaryCb == caller) {
                primaryCb = -1;
            } else if (secondaryCb == caller) {
                secondaryCb = -1;
            } else if (thirdCb == caller) {
                thirdCb = -1;
            } else {
                PANIC("deregisration failed");
            }
        } else {
            PANIC("cb - unknown IPC");
        }
    }
}

// subscribe to sensor updates
void RegisterCb() {
    v_Send_c(WhoIs("cbS"), CBS_REGISTER_CB);
}

// unsubscribe from sensor updates
void DeregisterCb() {
    v_Send_c(WhoIs("cbS"), CBS_DEREGISTER);
}

void FireCb(int sensorId) {
    char c[2];
    c[0] = CBS_FIRE_CB;
    c[1] = (char) sensorId;
    Send(WhoIs("cbS"), c, 2, 0x0, 0);
}
