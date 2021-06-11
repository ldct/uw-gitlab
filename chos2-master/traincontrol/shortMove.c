#include <clockServer.h>
#include <instrumentedStop.h>
#include <io.h>
#include <nameServer.h>
#include <shortMove.h>
#include <stdio.h>
#include <stdlib.h>
#include <tracknode.h>
#include <trainnum.h>
#include <trainServer2.h>
#include <getTravelVset.h>
#include <panic.h>
#include <getAcceleration.h>
#include <isqrt.h>

unsigned msToWaitForDistance(int trainNum, unsigned distance) {
    unsigned correctedDistance = distance;
    if (trainNum == 1) {
        if (distance < 410) {
            distance += 30;
        }
    }
    else if (trainNum == 79) {
        if (distance < 210) {
            distance += 30;
        }
    } else if (trainNum == 24) {
    } else {
        logf("no corrections done");
    }
    return isqrt(1000*(1000*correctedDistance/getAcceleration(trainNum)));
}

void shortMove(int trainNum, int dist) {
    ASSERT_IS_TRAINNUM(trainNum);

    int msToWait = msToWaitForDistance(trainNum, dist);

    int ticks = (msToWait + 5) / 10;

    int deciseconds = ticks/10;
    logf("shortmove: waiting for %d.%d seconds", deciseconds / 10, deciseconds % 10);

    Putc2(COM1, 14, trainNum);
    tsSetVset(trainNum, 14);

    Delay(WhoIs("cs"), ticks);

    instrumentedStop("sm", trainNum);
    tsSetVset(trainNum, 0);

}
void CLI_sm(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    int argTrainNum = atoi(user_input + offsets[1]);
    int argDist = atoi(user_input + offsets[2]);

    if (!isTrainnum(argTrainNum)) {
        logf("sm: not a trainnum - %d", argTrainNum);
        return;
    }

    shortMove(argTrainNum, argDist);
}
