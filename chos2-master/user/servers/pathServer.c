#include <io.h>
#include <tracknode.h>
#include <pathServer.h>
#include <graph.h>
#include <nameServer.h>
#include <ipc_codes.h>
#include <syslib.h>
#include <panic.h>
#include <map.h>
#include <pathfinding.h>
#include <reserve.h>
#include <trainnum.h>
#include <dijkstra.h>

void pathServer() {

    RegisterAs("pathS");

    Map pathBlocked;
    initializeMap(&pathBlocked);

    int caller;
    char requestBuf[9];

    while (1) {
        Receive(&caller, requestBuf, 10);

        if (requestBuf[0] == PATHS_PLANROUTE) {
            int i0 = *(int*)(requestBuf + 2);
            int i1 = *(int*)(requestBuf + 6);
            char trainNum = requestBuf[1];

            ASSERT_IS_TRAINNUM(trainNum);

            int res[100];
            dijkstra(trackNodeFromId(i0), trackNodeFromId(i1), res, &pathBlocked, 0);
            reverseList(res);

            int reservationFailed = 0;

            int numReserved = 0;
            for (int i = 0; res[i] != -1; i++) {
                reserve(trackNodeFromId(res[i]), &pathBlocked, (int) trainNum, &reservationFailed, &numReserved, 0);
            }

            if (reservationFailed) {
                logf("pathServer: reservation failed");
            } else {
                logf("pathserver: reserved %d", numReserved);
            }

            Reply(caller, res, 400);
        } else if (requestBuf[0] == PATHS_PLANROUTE_R) {
            int i0 = *(int*)(requestBuf + 2);
            int i1 = *(int*)(requestBuf + 6);
            char trainNum = requestBuf[1];

            ASSERT_IS_TRAINNUM(trainNum);

            int res[100];
            dijkstra(trackNodeFromId(i0), trackNodeFromId(i1), res, &pathBlocked, 1);
            reverseList(res);

            int reservationFailed = 0;

            int numReserved = 0;
            for (int i = 0; res[i] != -1; i++) {
                reserve(trackNodeFromId(res[i]), &pathBlocked, (int) trainNum, &reservationFailed, &numReserved, 0);
            }

            if (reservationFailed) {
                logf("pathServer: reservation failed");
            } else {
                logf("pathserver: reserved %d", numReserved);
            }

            Reply(caller, res, 400);
        } else if (requestBuf[0] == PATHS_RELEASEPATH) {
            Reply0(caller);

            char argTrainNum = requestBuf[1];

            ASSERT_IS_TRAINNUM(argTrainNum);

            logf("release argTrainNum=%d", argTrainNum);

            int r = freeAll(&pathBlocked, argTrainNum);

            logf("pathserver: released %d reservations", r);

        } else {
            PANIC("pathServer - unknown IPC");
        }

    }

}

void findPath(char trainNum, track_node* start, track_node* end, int out[100]) {
    c400_Send_ccii(WhoIs("pathS"), PATHS_PLANROUTE, trainNum, start->id, end->id, out);
}

void findPathR(char trainNum, track_node* start, track_node* end, int out[100]) {
    c400_Send_ccii(WhoIs("pathS"), PATHS_PLANROUTE_R, trainNum, start->id, end->id, out);
}

void releasePath(char trainNum) {
    v_Send_cc(WhoIs("pathS"), PATHS_RELEASEPATH, trainNum);
}
