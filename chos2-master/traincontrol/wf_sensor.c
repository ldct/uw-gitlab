#include <io.h>
#include <ipc_codes.h>
#include <tracknode.h>
#include <panic.h>
#include <syslib.h>
#include <cbServer.h>

void wf_Sensor_Task() {
    int creator;
    int sensorAddr;
    Receive(&creator, (char*) &sensorAddr, 4);
    RegisterCb();

    for (;;) {
        int caller;
        char str[2];
        Receive(&caller, str, 2);
        Reply0(caller);

        if (str[0] == TRAIN_SERVER_SENSOR_CB && str[1] == sensorAddr) {
            Reply0(creator);
            DeregisterCb();
            Exit();
        }
    }
}

void wf_Sensor(track_node* sensor) {
    ASSERT(sensor->type == NODE_SENSOR, "wf_Sensor: passed not a sensor");
    v_Send_i(Create(7, wf_Sensor_Task), sensor->num);
}
