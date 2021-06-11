#include <io.h>
#include <ipc_codes.h>
#include <marklin.h>
#include <graph.h>
#include <panic.h>
#include <trainData.h>
#include <syslib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <switchServer.h>
#include <wf_sensor.h>
#include <pathfinding.h>
#include <pathServer.h>
#include <trainServer2.h>
#include <panic.h>
#include <trainnum.h>
#include <getStopDistance.h>
#include <getSpeed.h>
#include <string.h>
#include <instrumentedStop.h>
#include <getTravelVset.h>
#include <dijkstra.h>
#include <getAcceleration.h>
#include <shortMove.h>
#include <tracking_globals.h>
#include <stdlib.h>
#include <getMinLmVset.h>

void setSwitchesFor(char trainNum, const int* path, int startIdx, int endIdx) {
    for (int i = startIdx; i < endIdx; i++) {
        track_node* br = trackNodeFromId(path[i]);
        if (br->type != NODE_BRANCH) continue;
        ASSERT(path[i+1] != -1, "path terminated with a branch");

        if (br->edge[DIR_STRAIGHT].dest->id == path[i+1]) {
            setTurnoutNB(br->num, POS_STRAIGHT);
        } else if (br->edge[DIR_CURVED].dest->id == path[i+1]) {
            setTurnoutNB(br->num, POS_CURVED);
        } else {
            logf("branch %s", br->name);
            PANIC("path has a branch followed by something with is not a branch target");
        }
    }
}

// does not use switch data
int distanceBetweenAdjacentNodes(track_node* node, track_node* next) {
    if (node->type == NODE_SENSOR || node->type == NODE_MERGE || node->type == NODE_ENTER || node->type == BROKEN_SENSOR) {
        if (node->edge[DIR_AHEAD].dest != next) {
            logf("not adjacent %s %s", node->name, next->name);
            PANIC("not adjacent");
        }
        ASSERT(node->edge[DIR_AHEAD].dest == next, "not adjacent");
        return node->edge[DIR_AHEAD].dist;
    } else if (node->type == NODE_BRANCH) {
        if (node->edge[DIR_CURVED].dest == next) return node->edge[DIR_CURVED].dist;
        if (node->edge[DIR_STRAIGHT].dest == next) return node->edge[DIR_STRAIGHT].dist;
        logf("not adjacent %s %s", node->name, next->name);
        PANIC("not adjacent");
    } else if (node->type == NODE_EXIT) {
        PANIC("exit has no distance to next node");
    } else {
        logf("node type %d %s", node->type, node->name);
        PANIC("distanceBetweenAdjacentNodes: unknown node type");
    }
}

int distanceOnPath(int* path, int startIdx, int endIdx) {
    ASSERT(startIdx >= 0, "distanceOnPath: startIdx > 0");
    ASSERT(endIdx < staLength(path), "distanceOnPath: endIdx < length");

    if (startIdx == endIdx) return 0;

    int ret = 0;
    int it = startIdx;

    for (int j=0;;j++) { ASSERT(j < 1000, "infinite loop detected");
        ASSERT(it+1 < staLength(path), "will overflow");
        ret += distanceBetweenAdjacentNodes(
            trackNodeFromId(path[it]),
            trackNodeFromId(path[it+1])
        );
        it++;
        if (it == endIdx) return ret;
    }
}

int findNextSensorOnPath(int* path, int startIdx) {
    ASSERT(startIdx >= 0, "findNextSensorOnPath, startIdx >= 0");
    ASSERT(startIdx < staLength(path), "findNextSensorOnPath startIdx < length");

    for (int it = startIdx + 1; it < 1000; it++) {
        if (path[it] == -1) return it;
        track_node* itn = trackNodeFromId(path[it]);
        if (itn->type == NODE_SENSOR) return it;
    }
    PANIC("infinite loop");
}

// only considers path[start:endIdx]
// path[endIdx-1] must point to a sensor
track_node* canLongMoveAtVset(int trainNum, int vset, int* path, int startIdx, int endIdx) {

    ASSERT(startIdx >= 0, "canLongMoveAtVset, startIdx >= 0");
    ASSERT(startIdx < staLength(path), "canLongMoveAtVset endIdx < length");
    ASSERT(startIdx <= endIdx, "canLongMoveAtVset: out of order");

    int stopDistance = getStopDistance(trainNum, vset);

    int it = startIdx;

    for (int j=0;;j++) { ASSERT(j < 1000, "infinite loop detected");

        int next = findNextSensorOnPath(path, it);
        if (next >= endIdx) return 0x0;

        int d1 = distanceOnPath(path, it, endIdx-1);
        int d2 = distanceOnPath(path, next, endIdx-1);

        if (d1 > stopDistance && d2 < stopDistance) {
            if (distanceOnPath(path, startIdx, it) > stopDistance) {
                return trackNodeFromId(path[it]);
            } else {
                return 0x0;
            }
        }

        if (it == endIdx || next == endIdx) {
            return 0x0;
        };
        it = next;
    }
}

int findMinimumLmVset(int trainNum, int* path, int startIdx, int endIdx) {
    for (int vset=14; vset >= getMinLmVset(trainNum); vset--) {
        if (canLongMoveAtVset(trainNum, vset, path, startIdx, endIdx)) return vset;
    }
    return 0;
}

void longMove(int trainNum, int vset, int overShoot, int* path, int startIdx, int endIdx) {
    ASSERT_IS_TRAINNUM(trainNum);
    ASSERT(vset >= 0, "vset");
    ASSERT(vset <= 14, "vset");

    int stopDistance = getStopDistance(trainNum, vset);
    track_node* it = canLongMoveAtVset(trainNum, vset, path, startIdx, endIdx);
    ASSERT(it, "but you said it could!");

    track_node* end = trackNodeFromId(path[endIdx-1]);

    // start the train
    Putc2(COM1, vset, trainNum);
    tsSetVset(trainNum, vset);

    logf("lm: wf_Sensor(%s)", it->name);
    wf_Sensor(it);
    int dist = distance(it, end);
    if (dist == -1) {
        logf("lm: distance()=-1");
        return;
    }
    dist += overShoot;

    int TRAIN_SPEED_MMS = getSpeed(trainNum, vset);
    int ticksToDelay = 100*(dist - stopDistance) / TRAIN_SPEED_MMS;
    ASSERT(ticksToDelay < 20000, "can't wait too long");


    if (ticksToDelay < 0) {
        logf("negative delay");
        ticksToDelay = 0;
    }

    logf("lm: delaying for %d ticks to get correct stopping point", ticksToDelay);

    Delay(WhoIs("cs"), ticksToDelay);

    instrumentedStop("lm", trainNum);
    tsSetVset(trainNum, 0);

    int stopTimeTicks = 100*getSpeed(trainNum, vset) / getAcceleration(trainNum);
    logf("lm: waiting %d ticks for train to actually stop", stopTimeTicks);
    Delay(WhoIs("cs"), stopTimeTicks);

}



int longMoveAtHighestVset(int trainNum, int overShoot, int* path, int startIdx, int endIdx) {
    logf("longMoveAtHighestVset(%d, %d)", startIdx, endIdx);
    int vset = findMinimumLmVset(trainNum, path, startIdx, endIdx);
    if (!vset) return 0;
    longMove(trainNum, vset, overShoot, path, startIdx, endIdx);
    logf("longMoveAtHighestVset return");
    return 1;
}


void doHaltAt(int trainNum, char* whereTo) {

    ASSERT_IS_TRAINNUM(trainNum);

    track_node* whereIsTheTrainNow = whereIsTrain(trainNum);

    if (whatVset(trainNum) != 0) {
        logf("haltat: train not stopped, refusing to doHaltAt");
        return;
    }

    /*
    (whereIsTheTrainNow)      train      (start)    .....     (end)
    */

    track_node* start = findNextSensor(whereIsTheTrainNow);
    track_node* end = trackNodeFromName(whereTo);

    logf("haltAt: %s -> %s", start->name, whereTo);

    int path[100];
    findPath(trainNum, start, end, path); // note: this marks the path as in use

    if (path[0] == -1) {
        logf("haltat: no path found\r\n");
        return;
    }

    char pathStr[1000];
    serializePath(pathStr, path);
    logf("path(%s,%s)=%s", start->name, end->name, pathStr);


    int vset = findMinimumLmVset(trainNum, path, 0, staLength(path));

    if (!vset) {
        logf("cannot long move at any speed above minimum LM speed");
        releasePath(trainNum);
        return;
    } else {
        logf("can long move at vset=%d", vset);
    }

    setSwitchesFor((char) trainNum, path, 0, staLength(path));

    longMove(trainNum, vset, 0, path, 0, staLength(path));

    releasePath(trainNum);
}

int areOppositeSensors(track_node* s1, track_node* s2) {
    ASSERT(s1, "java.lang.NullPointerException");
    ASSERT(s2, "java.lang.NullPointerException");
    if (s1->type != NODE_SENSOR) return 0;
    if (s2->type != NODE_SENSOR) return 0;
    return s1->reverse == s2;
}

/*

A B C S S' A -1
           ^
-----------|

A B C -1
       ^
--------
*/
int findNextReverseOrEnd(int* path, int startId) {
    int endIdx;
    for (endIdx=startId; endIdx<staLength(path); endIdx++) {
        if (endIdx > 0 && areOppositeSensors(trackNodeFromId(path[endIdx]), trackNodeFromId(path[endIdx-1]))) return endIdx+1;
    }
    return endIdx;
}

int countReverses(int* path) {
    int i, ret;
    for (i=0, ret=0; path[i] != -1; i = findNextReverseOrEnd(path, i), ret++) {
        ASSERT(ret < 100, "infinite loop detected");
    }
    return ret;
}

void stopPastSensor(int trainNum, int vset, track_node* sensor) {
    logf("stopPastSensor");
    Putc2(COM1, vset, trainNum);
    tsSetVset(trainNum, vset);
    logf("haltr: wf_Sensor(%s)...", sensor->name);
    wf_Sensor(sensor);
    logf("done");
    instrumentedStop("haltr", trainNum);


    int msToWait = 1000 * getSpeed(trainNum, vset) / (getAcceleration(trainNum));
    int ticks = (msToWait + 5) / 10;
    int deciseconds = ticks/10;
    logf("stopPastSensor: waiting for %d.%d seconds", deciseconds / 10, deciseconds % 10);

    Delay(WhoIs("cs"), ticks);
    tsSetVset(trainNum, 0);
}

void doHaltR(int trainNum, const char* whereTo, int silence) {

    ASSERT_IS_TRAINNUM(trainNum);

    track_node* whereIsTheTrainNow = whereIsTrain(trainNum);

    if (whatVset(trainNum) != 0) {
        logf("haltr: train not stopped, refusing to dohaltr");
        return;
    }

    /*
    (whereIsTheTrainNow)      train      (start)    .....     (end)
    */

    track_node* start = findNextSensor(whereIsTheTrainNow);
    track_node* end = trackNodeFromName(whereTo);

    int path[100];
    findPathR(trainNum, start, end, path);

    if (path[0] == -1) {
        if (!silence) {
            logf("haltr: no path found\r\n");
        }
        return;
    }

    logf("haltr: %s -> %s", whereIsTheTrainNow->name, whereTo);

    char pathBuf[1000];
    serializePath(pathBuf, path);
    logf("path(%s,%s)=%s", start->name, end->name, pathBuf);

    int curSegmentIdx = 0;

    while (1) {
        if (path[curSegmentIdx] == -1) {
            logf("done");
            return;
        };
        int endIdx = findNextReverseOrEnd(path, curSegmentIdx);

        logf("setSwitches(%d, %d)", curSegmentIdx, endIdx);
        setSwitchesFor((char) trainNum, path, curSegmentIdx, endIdx);

        if (endIdx >= 2 && areOppositeSensors(
            trackNodeFromId(path[endIdx-2]),
            trackNodeFromId(path[endIdx-1])
        )) {
            stopPastSensor(trainNum, 8, trackNodeFromId(path[endIdx-2])); // 8 = maximum speed with sd > 20cm
            v_Send_cc(WhoIs("scb"), TRAIN_SERVER_REVERSE_CB, (char) trainNum);
            Putc2(COM1, 15, trainNum);
            logf("hr: reversed");
            stopPastSensor(trainNum, 4, trackNodeFromId(path[endIdx-1])); // 4 = a nice slow speed
            curSegmentIdx = endIdx;
        } else {
            track_node* prevSensor = trackNodeFromId(path[0]);
            ASSERT(prevSensor->type == NODE_SENSOR, "path breakup - not a sensor 2");
            stopPastSensor(trainNum, 8, trackNodeFromId(path[endIdx-1])); // 8 = maximum speed with sd > 20cm
            break;
        }
    }
    releasePath(trainNum);
}

void haltAtTask() {
    int caller;
    char buf[100];

    Receive(&caller, buf, 100);
    Reply0(caller);

    int trainNum = (int) buf[0];
    char* whereTo = buf+1;

    doHaltAt(trainNum, whereTo);
}

void haltRTask() {
    int caller;
    char buf[100];

    Receive(&caller, buf, 100);
    Reply0(caller);

    int trainNum = (int) buf[0];
    char* whereTo = buf+1;

    doHaltR(trainNum, whereTo, 0);
}

void asyncHaltAt(int trainNum, const char* whereTo) {
    char buf[100];
    buf[0] = (char) trainNum;
    ASSERT(strlen(whereTo) < 10, "asyncHaltAt: whereTo too long");
    strcpy(buf+1, whereTo);
    Send(Create(100, haltAtTask), buf, 100, 0x0, 0);
}

void asyncHaltR(int trainNum, const char* whereTo) {
    char buf[100];
    buf[0] = (char) trainNum;
    ASSERT(strlen(whereTo) < 10, "asyncHaltR: whereTo too long");
    strcpy(buf+1, whereTo);
    Send(Create(100, haltRTask), buf, 100, 0x0, 0);
}

// todo: headers
extern int seedSeed;
unsigned lrand(int* seed);

void haltRLoop(int trainNum) {
    int seed = seedSeed + trainNum;
    for (int j=0;;j++) {
        if (j % 100 == 0) {
            logf("haltRLoop: iteration %d", j);
        }
        int targetId = lrand(&seed) % NUM_NODES;
        track_node* target = trackNodeFromId(targetId);
        if (target->type != NODE_SENSOR) continue;
        if (target->edge[DIR_AHEAD].dest->type == NODE_EXIT) continue;
        logf("haltRLoop: go to %s", target->name);
        doHaltR(trainNum, target->name, 1);
    }
}


void haltRLoopTask() {
    char trainNum = c_Receive_v();
    ASSERT_IS_TRAINNUM(trainNum);
    haltRLoop((int)trainNum);
    logf("haltRTask: bye");
}

void asyncHaltRLoop(int trainNum) {
    v_Send_c(Create(100, haltRLoopTask), (char)trainNum);
}

void CLI_hl(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    // haltRLoop(atoi(use_input + offsets[1]));
    asyncHaltRLoop(atoi(user_input + offsets[1])); // <- train 1 on track B works pretty well
}
