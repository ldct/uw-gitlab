#include <tracknode.h>
#include <minHeap.h>
#include <trackdata.h>
#include <graph.h>
#include <tracking_globals.h>
#include <map.h>
#include <dijkstra.h>
#include <panic.h>
#include <reserve.h>
#include <io.h>

// the cost of reversing, in mm
#define REVERSE_COST 1

void considerEdge(
    track_node* u, track_node* v, int edge_dist,
    const Map* reservations,
    int dist[TRACK_MAX], track_node* prev[TRACK_MAX],
    MinHeap* Q
) {

    int alt = dist[u->id] + edge_dist;

    if (isFree(reservations, v) && alt < dist[v->id]) {
        dist[v->id] = alt;
        prev[v->id] = u;
        insertMinHeap(Q, alt, v);
    }

}

int edgeIsBlocked(track_edge* edge) {
    return (
        edge->dest->type == NODE_BRANCH ||
        edge->dest->type == NODE_MERGE ||
        edge->dest->type == NODE_ENTER ||
        edge->dest->type == NODE_EXIT
    ) && edge->dist < 210;
}

int canReverseOn(track_node* sensor) {
    if (sensor->type == BROKEN_SENSOR) return 0;
    ASSERT(sensor->type == NODE_SENSOR, "bad sensor");
    if (edgeIsBlocked(&(sensor->reverse->edge[DIR_AHEAD]))) return 0;
    if (edgeIsBlocked(&(sensor->edge[DIR_AHEAD]))) return 0;
    return 1;
}

void dijkstra(track_node* source, track_node* dest, int* out, const Map* reservations, int allowReverse) {
    MinHeap Q;
    initializeMinHeap(&Q);

    int dist[TRACK_MAX];
    track_node* prev[TRACK_MAX];

    dist[source->id] = 0;
    insertMinHeap(&Q, 0, source);

    for (int v=0; v<NUM_NODES; v++) {
        if (v != source->id) {
            dist[v] = INF;
        }
        prev[v] = 0x0;
    }

    while (peek(&Q)) {
        track_node* u = removeMinHeap(&Q);

        if (u->type == NODE_SENSOR || u->type == BROKEN_SENSOR) {
            considerEdge(u, u->edge[DIR_AHEAD].dest, u->edge[DIR_AHEAD].dist, reservations, dist, prev, &Q);
            if (allowReverse && canReverseOn(u)) {
                considerEdge(u, u->reverse, REVERSE_COST, reservations, dist, prev, &Q);
            }
        } else if ( u->type == NODE_MERGE || u->type == NODE_ENTER || u->type == BROKEN_SENSOR) {
            considerEdge(u, u->edge[DIR_AHEAD].dest, u->edge[DIR_AHEAD].dist, reservations, dist, prev, &Q);
        } else if (u->type == NODE_BRANCH) {
            considerEdge(u, u->edge[DIR_STRAIGHT].dest, u->edge[DIR_STRAIGHT].dist, reservations, dist, prev, &Q);
            considerEdge(u, u->edge[DIR_CURVED].dest, u->edge[DIR_CURVED].dist, reservations, dist, prev, &Q);
        } else if (u->type == NODE_EXIT) {
            continue;
        } else {
            logf("unknown next node: %d", u->type);
            PANIC("unknown next node 3");
        }
    }

    if (dist[dest->id] >= INF) {
        out[0] = -1;
        return;
    }
    track_node* u = dest;
    int i = 0;
    if (prev[u->id] || u == source) {
        while (u) {
            out[i++] = u->id;
            u = prev[u->id];
        }
    }
    out[i-1] = source->id;
    out[i] = -1;
}
