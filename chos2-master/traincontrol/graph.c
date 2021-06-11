#include <graph.h>
#include <trackdata.h>
#include <bwio.h>
#include <panic.h>
#include <io.h>
#include <switchServer.h>
#include <tracking_globals.h>
#include <stdio.h>

void FW_initializePathAndCost(int graph[TRACK_MAX][TRACK_MAX]) {
    for (int v = 0; v < TRACK_MAX; v++) {
        for (int u = 0; u < TRACK_MAX; u++) {
            cost[v][u] = graph[v][u];

            if (v == u)
				path[v][u] = 0;
			else if (cost[v][u] != INF)
				path[v][u] = v;
			else
				path[v][u] = -1;
        }
    }
    for (int k = 0; k < TRACK_MAX; k++) {
        for (int v = 0; v < TRACK_MAX; v++) {
            for (int u = 0; u < TRACK_MAX; u++) {

                if (cost[v][k] != INF && cost[k][u] != INF
					&& cost[v][k] + cost[k][u] < cost[v][u])
				{
					cost[v][u] = cost[v][k] + cost[k][u];
					path[v][u] = path[k][u];
				}
            }
            ASSERT(cost[v][v] >= 0, "negative weight cycle");
        }
    }
}

void printCost(int v, int u) {
    lprintf(COM2, "cost=%d\r\n", cost[v][u]);
}

int staLength(int* path) {
    int i;
    for (i=0; path[i] != -1; i++) {}
    return i;
}

void reverseList(int a[]) {
    int i = staLength(a) - 1;
    int j = 0;
    while(i > j) {
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
        i--;
        j++;
  }
}

void FW_writePath(int v, int u, int* out) {

    if (FW_getCost(v, u) >= INF) {
        out[0] = -1;
        return;
    }

    if (v == u) {
        *out = u;
        *(out+1) = -1;
        return;
    }

    int nextId = path[v][u];

    *out = u;
	FW_writePath(v, nextId, out + 1);
}

void serializePath(char* buf, int a[]) {
    char* p = buf;
    p += sprintf(p, "< ");
    for (int i=0; a[i] != -1; i++) {
        p += sprintf(p, "%s ", nodes[a[i]].name);
    }
    p += sprintf(p, ">");
    *p = 0;
}

int FW_getCost(int i, int j) {
    return cost[i][j];
}
