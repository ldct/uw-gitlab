#ifndef TRACKING_GLOBALS_H
#define TRACKING_GLOBALS_H

#include <trackdata.h>

extern int cost[TRACK_MAX][TRACK_MAX];
extern int path[TRACK_MAX][TRACK_MAX];
extern int graph[TRACK_MAX][TRACK_MAX];
extern track_node nodes[TRACK_MAX];

extern int NUM_NODES;

#endif
