#include <io.h>
#include <panic.h>
#include <marklin.h>
#include <ipc_codes.h>
#include <tracknode.h>
#include <pathfinding.h>
#include <switchServer.h>
#include <trainData.h>
#include <syslib.h>
#include <timer.h>
#include <nameServer.h>
#include <hud.h>
#include <printTrainPosition.h>
#include <cbServer.h>
#include <map.h>
#include <trainStatus.h>
#include <trainServerUi.h>
#include <pathServer.h>
#include <tracking_globals.h>
#include <reserve.h>
#include <getStopDistance.h>
#include <trainnum.h>

void initializeTrainStatus(TrainStatus* trainStatus) {
    trainStatus->trainNum = 0;
    trainStatus->vset = 0;
    trainStatus->last_tripped = 0x0;
    trainStatus->last_tripped_time = 0;
    trainStatus->prev_tripped = 0x0;
    trainStatus->prev_tripped_time = 0;
    trainStatus->is_reversing = 0;
    initializeQueue("invspeeds", &(trainStatus->invspeeds));
}

int sensorAttributable(TrainStatus* trainStatus, track_node* this_tripped) {
    if (trainStatus->trainNum == 0) return 0;
    if (!(trainStatus->last_tripped)) return 0;
    if (trainStatus->is_reversing && this_tripped == trainStatus->last_tripped->reverse) return 1;
    return findNextSensor(trainStatus->last_tripped) == this_tripped;
}

int sensorExpected(TrainStatus* trainStatus, track_node* this_tripped) {
    if (trainStatus->trainNum == 0) return 0;
    if (!(trainStatus->last_tripped)) return 1;
    if (trainStatus->is_reversing && this_tripped == trainStatus->last_tripped->reverse) return 1;
    return findNextSensor(trainStatus->last_tripped) == this_tripped;
}

void trainServer2() {
    RegisterAs("scb");
    RegisterCb();

    Create(10, setInitialSwitches);

    Map reservations;
    initializeMap(&reservations);

    TrainStatus trains[2];
    const int numTrains = 2;

    for (int i=0; i<numTrains; i++) {
        initializeTrainStatus(&trains[i]);
        printStatusLine(i, &trains[i]);
    }

    for (;;) {

        int caller;
        char str[3];
        Receive(&caller, str, 3);

        if (str[0] == TRAIN_SERVER_SENSOR_CB ) {
            Reply0(caller);
            track_node* this_tripped = sensorNodeFromAddress(str[1]);

            // todo: track train order
            for (int i=0; i<numTrains; i++) {
                if (trains[i].last_tripped == this_tripped) logf("warn: not debounced or two trains in the same segment");
            }

            if (sensorAttributable(&trains[0], this_tripped) && sensorAttributable(&trains[1], this_tripped)) {
                log("warn: triggered sensor is attributable for both trains, reservation system failed");
            }

            int handled = 0;
            for (int trainId=0; !handled && trainId<numTrains; trainId++) {
                if (sensorExpected(&trains[trainId], this_tripped)) {

                    if (trains[trainId].is_reversing) {
                        logf("1. caught the reversing at this_tripped=%s", this_tripped->name);
                        trains[trainId].is_reversing = 0;
                    }

                    handled = 1;

                    int now = getValue(3);

                    /*
                    Reservations
                    */

                    // release all reservations
                    freeAll(&reservations, trainId);

                    // reserve from [this_tripped, next_tripped)

                    int needToSlow = 0;

                    track_node* it = this_tripped;
                    track_node* next_tripped = findNextSensor(this_tripped);

                    for (int j=0;;j++) {
                        ASSERT(j < 1000, "infinite loop detected");

                        reserve(it, &reservations, trainId, &needToSlow, 0x0, 0);

                        if (it == next_tripped) {
                            break;
                        } else {
                            track_node* next = findNextNode(it);
                            if (it == next) break;
                            it = next;
                        }
                    }

                    // reserve [next_tripped, findNextSensor(next_tripped + stop_distance))

                    it = next_tripped;

                    for (int j=0;;j++) {
                        ASSERT(j < 1000, "infinite loop detected");

                        reserve(it, &reservations, trainId, &needToSlow, 0x0, 0);

                        if (distance(next_tripped, it) > getStopDistance(trains[trainId].trainNum, trains[trainId].vset)) {
                            break;
                        } else {
                            track_node* next = findNextNode(it);
                            if (it == next) break;
                            it = next;
                        }
                    }

                    for (int j=0;;j++) {
                        ASSERT(j < 1000, "infinite loop detected");

                        reserve(it, &reservations, trainId, &needToSlow, 0x0, 0);
                        if (it->type == NODE_SENSOR) {
                            break;
                        } else {
                            track_node* next = findNextNode(it);
                            if (it == next) break;
                            it = next;
                        }
                    }

                    // check if we need to slow

                    if (needToSlow) {
                        logf("tried to reserve an already reserved sensor. halt!");
                        trains[trainId].vset = 0;
                        Putc2NB(COM1, (char) trains[trainId].vset, (char) trains[trainId].trainNum);
                    }

                    trains[trainId].prev_tripped = trains[trainId].last_tripped;
                    trains[trainId].prev_tripped_time = trains[trainId].last_tripped_time;
                    trains[trainId].last_tripped = this_tripped;
                    trains[trainId].last_tripped_time = now;

                    printStatusLine(trainId, &trains[trainId]);
                }
            }

            if (!handled) {
                if (this_tripped && this_tripped->type == BROKEN_SENSOR) {
                    logf("a broken sensor %s was tripped", this_tripped->name);
                } else {
                    logf("WARN: sensor %s tripped out of order", this_tripped->name);
                }
            }

            drawTrainPosition(trains[0].last_tripped, trains[1].last_tripped);

        } else if (str[0] == TRAIN_SERVER_WHERE) {
            int argTrainNum = (int) str[1];
            if (trains[0].trainNum == argTrainNum) {
                v_Reply_i(caller, (int) trains[0].last_tripped);
            } else if (trains[1].trainNum == argTrainNum) {
                v_Reply_i(caller, (int) trains[1].last_tripped);
            } else {
                logf("WARN: no trains found. caller will now be a zombie");
            }
        } else if (str[0] == TRAIN_SERVER_REVERSE_CB) {
            Reply0(caller);

            int argTrainNum = (int) str[1];

            logf("mark trains %d as reversing", argTrainNum);

            if (trains[0].trainNum == argTrainNum) {
                trains[0].is_reversing = 1;
            } else if (trains[1].trainNum == argTrainNum) {
                trains[1].is_reversing = 1;
            } else {
                logf("warn: no trains with number %d", argTrainNum);
            }
        } else if (str[0] == TRAIN_SERVER_WHAT_VSET) {
            int argTrainNum = (int) str[1];

            if (trains[0].trainNum == argTrainNum) {
                v_Reply_i(caller, trains[0].vset);
            } else if (trains[1].trainNum == argTrainNum) {
                v_Reply_i(caller, trains[1].vset);
            } else {
                logf("warn: no trains with number %d", argTrainNum);
                v_Reply_i(caller, 14);
            }
        } else if (str[0] == TRAIN_SERVER_VSET_CB) {
            Reply0(caller);
            int argVset = (int) str[1];
            int argTrainNum = (int) str[2];

            if (trains[0].trainNum == 0 || trains[0].trainNum == argTrainNum) {
                trains[0].vset = argVset;
                trains[0].trainNum = argTrainNum;
                // todo - hmm. should not be statusLine1 always
                printStatusLine(0, &trains[0]);
            } else if (trains[1].trainNum == 0 || trains[1].trainNum == argTrainNum) {
                trains[1].vset = argVset;
                trains[1].trainNum = argTrainNum;
                printStatusLine(1, &trains[1]);
            } else {
                logf("warn: all trackers full, not tracking trains %d", argTrainNum);
            }
        } else {
            PANIC("trainServer - unknown IPC");
        }

    }
}

track_node* whereIsTrain(int trainNum) {
    ASSERT_IS_TRAINNUM(trainNum);
    track_node* ret = (track_node*) i_Send_cc(WhoIs("scb"), TRAIN_SERVER_WHERE, (char)trainNum);
    ASSERT(ret->type == NODE_SENSOR, "TRAIN_SERVER_WHERE did not return a sensor");
    return ret;
}

int whatVset(int trainNum) {
    ASSERT_IS_TRAINNUM(trainNum);
    return i_Send_cc(WhoIs("scb"), TRAIN_SERVER_WHAT_VSET, (char)trainNum);
}

void tsSetVset(int trainNum, int speed) {
    v_Send_ccc(WhoIs("scb"), TRAIN_SERVER_VSET_CB, (char) speed, (char) trainNum);
}
