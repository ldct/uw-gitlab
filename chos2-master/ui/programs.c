#include <stdlib.h>
#include <stdio.h>
#include <switchServer.h>
#include <programs.h>
#include <tracknode.h>
#include <map.h>
#include <graph.h>
#include <io.h>
#include <pathfinding.h>
#include <marklin.h>
#include <syslib.h>
#include <wf_sensor.h>
#include <haltVisit.h>
#include <trainServer2.h>
#include <measureStopDistance.h>
#include <stopAllTrains.h>
#include <timer.h>
#include <ts7200.h>
#include <interrupt.h>
#include <ipc_codes.h>
#include <nameServer.h>
#include <doReverse.h>
#include <trainnum.h>
#include <instrumentedStop.h>
#include <clockServer.h>
#include <dijkstra.h>
#include <tracking_globals.h>
#include <isqrt.h>
#include <pathfinding.h>
#include <string.h>
#include <cbServer.h>

void CLI_q(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    stopAllTrains();
    logf("all trains stopped");
    v_Send_c(WhoIs("com1TxS"), UART1_SHUT_DOWN);
    disableTimer(1);
    disableTimer(3);
    setEnabledDevices(0x0, 0x0);
    KillKernel();
}

void CLI_ut(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    if (
        (0 == strcmp(user_input + offsets[1], "a")) ||
        (0 == strcmp(user_input + offsets[1], "A"))
    ) {
        initAndAdjustA();
    } else if (
        (0 == strcmp(user_input + offsets[1], "b")) ||
        (0 == strcmp(user_input + offsets[1], "B"))
    ) {
        initAndAdjustB();
    } else {
        logf("ut - unrecognised track");
    }
}

void CLI_sw(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    if (
        (0 == strcmp(user_input + offsets[2], "s")) ||
        (0 == strcmp(user_input + offsets[2], "S"))
    ) {
        // todo: turn off
        setTurnout(atoi(user_input + offsets[1]), POS_STRAIGHT);
    } else if (
        (0 == strcmp(user_input + offsets[2], "c")) ||
        (0 == strcmp(user_input + offsets[2], "C"))
    ) {
        setTurnout(atoi(user_input + offsets[1]), POS_CURVED);
    } else {
        logf("sw - unrecognised direction");
    }
}

void CLI_rv(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    int trainNum = atoi(user_input + offsets[1]);
    doReverse(trainNum);
}

void sc(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    // todo: turn off
    setTurnout(atoi(user_input + offsets[1]), POS_CURVED);
}

void ss(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    setTurnout(atoi(user_input + offsets[1]), POS_STRAIGHT);
}

void CLI_add(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    int a = atoi(user_input + offsets[1]);
    int b = atoi(user_input + offsets[2]);
    logf("%d + %d = %d", a, b, a + b);
}

int countReverses(int* path);

void CLI_rt(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    int path[100];

    track_node* start = trackNodeFromName(user_input + offsets[1]);
    track_node* end = trackNodeFromName(user_input + offsets[2]);

    dijkstra(start, end, path, (Map*) 0x0, 0);
    reverseList(path);
    char pathBuf[1000];
    serializePath(pathBuf, path);
    logf("dijk:\t%s", pathBuf);

    dijkstra(start, end, path, (Map*) 0x0, 1);
    reverseList(path);
    serializePath(pathBuf, path);
    logf("dijk+r:\t%s", pathBuf);

    logf("# reverses: %d", countReverses(path));

    FW_writePath(start->id, end->id, path);
    reverseList(path);
    serializePath(pathBuf, path);
    logf("floyd:\t%s", pathBuf);
}

int findNextReverseOrEnd(int* path, int startId);

void findLongPathsR() {
    int path[100];

    int maxNr = 0;

    for (int j=0; j<NUM_NODES; j++) {

        track_node* start = trackNodeFromName("C7");
        track_node* end = trackNodeFromId(j);
        dijkstra(start, end, path, (Map*) 0x0, 1);
        reverseList(path);

        int nr = countReverses(path);

        if (nr > maxNr) {
            logf("%s -> %s requires %d reverses", start->name, end->name, nr);
            maxNr = nr;
        }

    }
}

int canReverseOn(track_node* sensor);
unsigned msToWaitForDistance(int trainNum, unsigned distance);
void countReversibleSensors() {
    int numCanReverse = 0;
    int numCannotReverse = 0;

    for (int i=0; i<NUM_NODES; i++) {
        track_node* node = trackNodeFromId(i);
        if (node->type != NODE_SENSOR) continue;

        if (canReverseOn(node)) {
            logf("can reverse on %s with clearance %d", node->name, node->edge[DIR_AHEAD].dist);
            numCanReverse++;
        } else {
            numCannotReverse++;
        }
    }
    logf("can reverse on %d but not on %d", numCanReverse, numCannotReverse);
}

track_node* canLongMoveAtVset(int trainNum, int vset, int* path, int startIdx, int endIdx);

int findNextSensorOnPath(int* path, int startIdx);

void tfr(const char* user_input, int offsets[MAX_ARGUMENTS]) {

    countReversibleSensors();

}

void halthalt(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    int argTrain1 = atoi(user_input + offsets[1]);
    const char* argWhereTo1 = user_input + offsets[2];
    int argTrain2 = atoi(user_input + offsets[3]);
    const char* argWhereTo2 = user_input + offsets[4];
    logf("halthalt: asyncHaltAt(%d,%s)", argTrain1, argWhereTo1);
    asyncHaltAt(argTrain1, argWhereTo1);
    logf("halthalt: asyncHaltAt(%d,%s)", argTrain2, argWhereTo2);
    asyncHaltAt(argTrain2, argWhereTo2);
}

void CLI_tr(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    int trainNum = atoi(user_input + offsets[1]);
    int speed = atoi(user_input + offsets[2]);

    if (!isTrainnum(trainNum)) {
        logf("tr: not a trainnum - %d", trainNum);
        return;
    }

    if (speed > 14) {
        logf("tr: not a speed - %d", speed);
        return;
    }

    Putc2(COM1, speed, trainNum);
    tsSetVset(trainNum, speed);
}

void CLI_lm(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    asyncHaltAt(atoi(user_input + offsets[1]), user_input + offsets[2]);
}

void CLI_hr(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    asyncHaltR(atoi(user_input + offsets[1]), user_input + offsets[2]);
}

void CLI_sd(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    measureStopDistance(user_input + offsets[1]);
}

void triggerSensor(const char* name) {
    track_node* sensor = trackNodeFromName(name);
    FireCb(sensor->num);
}

void CLI_ts(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    triggerSensor(user_input + offsets[1]);
}
