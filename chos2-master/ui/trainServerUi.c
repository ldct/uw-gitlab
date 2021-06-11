#include <io.h>
#include <trainStatus.h>
#include <pathfinding.h>
#include <trainData.h>
#include <trainServerUi.h>
#include <getSpeed.h>
#include <panic.h>
#include <hud.h>

void clearStatusLine(int linenum) {
    lprintf(
        COM2,
        "\033[s\033[%d;1H                                                                                \033[u",
        linenum
    );
}

void printStatusLine1(int linenum, TrainStatus* trainStatus) {
    clearStatusLine(linenum);
    lprintf(
        COM2,
        "\033[s\033[%d;1Htrain=%d  \t? -> ?\tvset=%d\033[u",
        linenum,
        trainStatus->trainNum,
        trainStatus->vset
    );
}

void printStatusLine2(int linenum, TrainStatus* trainStatus) {
    clearStatusLine(linenum);
    lprintf(
        COM2,
        "\033[s\033[%d;1Htrain=%d  \t%s -> %s\tvset=%d\033[u",
        linenum,
        trainStatus->trainNum,
        trainStatus->last_tripped->name,
        findNextSensor(trainStatus->last_tripped)->name,
        trainStatus->vset
    );
}

// note: pushes into invspeeds as well
void printStatusLine3(int linenum, TrainStatus* trainStatus) {

    if (trainStatus->is_reversing) {
        clearStatusLine(linenum);
        lprintf(
            COM2,
            "\033[s\033[%d;1Htrain=%d  %s->REVERSING      \033[u",
            linenum,
            trainStatus->trainNum,
            trainStatus->last_tripped->name
        );
        return;
    }
    unsigned lasttime = trainStatus->prev_tripped_time - trainStatus->last_tripped_time;
    int lastdist = distance(trainStatus->prev_tripped, trainStatus->last_tripped);

    if (lastdist == -1) {
        logf("psl3: distance=-1");
        return;
    }

    int invspeed = lasttime / lastdist;
    push(&(trainStatus->invspeeds), (void*) invspeed);

    int speed6 = 0;

    int total_v = 0; // total_invspeed
    if (ringFill(&(trainStatus->invspeeds)) > INVSPEED_SAMPLES) {
        pop(&(trainStatus->invspeeds));
        for (int it = trainStatus->invspeeds.cursor; it < trainStatus->invspeeds.length; it++) {
            int it_invspeed = (int) trainStatus->invspeeds.queue[ring(it)];
            total_v += 508000/it_invspeed;
        }
        speed6 = total_v / INVSPEED_SAMPLES;
    } else {
    }

    int errorEstimate = lasttime/508 - 1000*lastdist/getSpeed(trainStatus->trainNum, trainStatus->vset);
    clearStatusLine(linenum);
    lprintf(
        COM2,
        "\033[s\033[%d;1Htrain=%d  \t%s -> %s\tvset=%d\terror=%dms\tspeed(6)=%d      \033[u",
        linenum,
        trainStatus->trainNum,
        trainStatus->last_tripped->name,
        findNextSensor(trainStatus->last_tripped)->name,
        trainStatus->vset,
        errorEstimate,
        speed6
    );
}

// todo: headers
int areOppositeSensors(track_node* s1, track_node* s2);

void printStatusLine(int trainId, TrainStatus* trainStatus) {
    ASSERT(trainId == 0 || trainId == 1, "not a trainId");

    int linenum = trainId == 0 ? TRAINSERVER_LINENUM : TRAINSERVER_LINENUM2;

    if (!(trainStatus->last_tripped) && !(trainStatus->prev_tripped)) return printStatusLine1(linenum, trainStatus);
    if (trainStatus->last_tripped && !(trainStatus->prev_tripped)) return printStatusLine2(linenum, trainStatus);
    if (trainStatus->last_tripped && trainStatus->prev_tripped) {
        if (areOppositeSensors(trainStatus->last_tripped, trainStatus->prev_tripped)) {
            return printStatusLine2(linenum, trainStatus);
        } else {
            return printStatusLine3(linenum, trainStatus);
        }
    }

    logf("warn: printstatusline - nothing matched");
}
