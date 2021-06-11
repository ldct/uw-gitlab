#include <reserve.h>
#include <tracknode.h>
#include <map.h>
#include <io.h>
#include <tracking_globals.h>

int freeAll(Map* reservations, int trainKey) {
    int r = 0;
    for (int j=0; j<NUM_NODES; j++) {
        int whoHasTheReservation = (int) getMap(reservations, j);
        if (whoHasTheReservation == trainKey+1) {
            r++;
            removeMap(reservations, j);
        }
    }
    return r;
}

int isFree(const Map* reservation, const track_node* node) {
    if (!reservation) return 1;
    int ret = (int) getMap(reservation, node->id) == 0;
    return ret;
}

void reserve(track_node* node, Map* reservationMap, int trainId, int* reservationFailed, int* numReserved, int logReservations) {

    int toReserve = node->id;
    int whoHasTheReservation = (int) getMap(reservationMap, toReserve);
    if (whoHasTheReservation == 0 || whoHasTheReservation == trainId+1) {
        putMap(reservationMap, toReserve, (void*) (trainId+1));
        if (logReservations) logf("reserved: <%s> ", node->name);
        if (numReserved) (*numReserved)++;
    } else {
        *reservationFailed = 1;
    }

    int toReserve2 = node->reverse->id;
    int whoHasTheReservation2 = (int) getMap(reservationMap, toReserve2);
    if (whoHasTheReservation2 == 0 || whoHasTheReservation2 == trainId+1) {
        putMap(reservationMap, toReserve2, (void*) (trainId+1));
        if (logReservations) logf("reserved: <%s>", node->name);
        if (numReserved) (*numReserved)++;
    } else {
        *reservationFailed = 1;
    }
}
