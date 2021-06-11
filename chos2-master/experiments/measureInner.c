#include <io.h>
#include <panic.h>
#include <marklin.h>
#include <timer.h>
#include <estimator.h>
#include <ipc_codes.h>
#include <cbServer.h>
#include <syslib.h>
#include <clockServer.h>
#include <nameServer.h>

void doMeasurement(int vset) {

    const int sensors[] = {
        21, // B6
        61, // D4
    };
    const int num_sensors = 2;

    Estimator estimators[num_sensors];
    for (int i=0; i<num_sensors; i++) {
        initializeEstimator(&(estimators[i]));
    }

    lprintf(COM2, "starting vset=%d\r\n", vset);

    Putc2(COM1, vset, 74);

    int last_tripped = -1;

    unsigned int time1 = 0;
    unsigned int time2 = 0;

    int rounds = 0;

    for (;;) {
        int caller;
        char str[2];
        Receive(&caller, str, 2);
        Reply0(caller);

        ASSERT(str[0] == TRAIN_SERVER_SENSOR_CB, "unexpected IPC");
        char c = str[1];

        if (last_tripped == -1) {
            if (c == sensors[0]) {
                last_tripped = 0;
                time1 = getValue(3);
                lprintf(COM2, "last_tripped=%d\r\n", last_tripped);
            } else {
                continue;
            }
        } else {
            int expected_next = (last_tripped + 1) % num_sensors;
            if (!(c == sensors[expected_next] || c == sensors[last_tripped])) {
                continue;
            }
            if (c == sensors[expected_next]) {
                if (last_tripped == 0) {
                    if (rounds == NUM_SAMPLES) {
                        break;
                    } else {
                        rounds++;
                    }
                }
                time2 = getValue(3);
                // lprintf(COM2, "inserting into estimator %d\r\n", expected_next);
                insertSample(&(estimators[expected_next]), time1 - time2);
                last_tripped = expected_next;
                lprintf(COM2, "last_tripped=%d\ttrip_time=%u\trounds=%d\tvset=%d\r\n", last_tripped, time1 - time2, rounds, vset);
                time1 = time2;
            }
        }

    }

    lprintf(COM2, "experiment done\r\n");

    for (int i=0; i<num_sensors; i++) {
        int mean = produceEstimate(&(estimators[i]));
        lprintf(COM2, "estimator %d \t %d\r\n", i, mean);
    }

    // Halt den Zug an
    Putc2(COM1, 0, 74);

    // 2 seconds
    Delay(WhoIs("cs"), 200);

    lprintf(COM2, "train stopped\r\n");
}

void measureInner() {
    RegisterCb();
    doMeasurement(14);
    lprintf(COM2, "all done\r\n");
}
