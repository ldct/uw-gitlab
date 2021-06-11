#ifndef RESERVE_H
#define RESERVE_H

#include <tracknode.h>
#include <map.h>

int freeAll(Map* reservation, int trainKey);
void reserve(track_node* node, Map* map, int trainKey, int* reservationFailed, int* numReserved, int logReservations);
int isFree(const Map* reservation, const track_node* node);

#endif
