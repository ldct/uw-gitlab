#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <map.h>

void dijkstra(track_node* source, track_node* dest, int* out, const Map* reservations, int allowReverse);

#endif
