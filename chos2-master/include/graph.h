#ifndef GRAPH_H
#define GRAPH_H

#define INF 0x0FFFFF
#define TRACK_MAX 144

#include <tracknode.h>

// written once by init() and then read-only
extern int graph[TRACK_MAX][TRACK_MAX];

void FW_initializePathAndCost(int graph[TRACK_MAX][TRACK_MAX]);

int FW_getCost(int i, int j);

void serializePath(char* buf, int a[]);
void FW_writePath(int v, int u, int* out);
void reverseList(int a[]);

int staLength(int* path);

#endif
