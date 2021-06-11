#include <io.h>
#include <syslib.h>
#include <ipc_codes.h>
#include <nameServer.h>
#include <clockServer.h>
#include <panic.h>
#include <trainServer2.h>
#include <instrumentedStop.h>

void Reverse_Task() {
    char trainNum = c_Receive_v();

    int trainVset = whatVset(trainNum);

    instrumentedStop("rv", trainNum);

    v_Send_ccc(WhoIs("scb"), TRAIN_SERVER_VSET_CB, trainVset/2, (char) trainNum);

    logf("rv: waiting for stop");

    // fixed 3s delay
    Delay(WhoIs("cs"), 300);

    // todo: check if the train is on a sensor

    logf("rv: informing ts of intention to reverse");

    v_Send_cc(WhoIs("scb"), TRAIN_SERVER_REVERSE_CB, (char) trainNum);

    logf("rv: reversing");

    Putc2(COM1, 15, trainNum);
    Delay(WhoIs("cs"), 10);

    logf("rv: back to normal speed");

    Putc2(COM1, trainVset, trainNum);

    v_Send_ccc(WhoIs("scb"), TRAIN_SERVER_VSET_CB, trainVset, (char) trainNum);

    logf("rv: done");
}

void doReverse(int trainNum) {
    v_Send_c(Create(100, Reverse_Task), (char) trainNum);
}
