#include <tracking_globals.h>
#include <graph.h>
#include <io.h>
#include <graph.h>
#include <trackdata.h>
#include <bwio.h>
#include <panic.h>
#include <io.h>
#include <switchServer.h>
#include <tracking_globals.h>
#include <pathfinding.h>
#include <string.h>

track_node* trackNodeFromId(int id) {
    return &nodes[id];
}

track_node* trackNodeFromName(const char* name) {
    for (int i=0; i<NUM_NODES; i++) {
        if (0 == strcmp(nodes[i].name, name)) {
            return &(nodes[i]);
        }
    }
    logf("name: %s", name);
    PANIC("trackNodeFromName name not found");
}

track_node* sensorNodeFromAddress(int sensorAddress) {
    for (int i=0; i<NUM_NODES; i++) {
        if (
            (nodes[i].type == NODE_SENSOR || nodes[i].type == BROKEN_SENSOR) &&
            nodes[i].num == sensorAddress) {
            return &(nodes[i]);
        }
    }
    logf("sensor not found: %d", sensorAddress);
    PANIC("sensor not found");
}

track_node* findNextNode(track_node* node) {
    ASSERT(node, "java.lang.nullpointerexception");
    if (node->type == NODE_SENSOR || node->type == NODE_MERGE || node->type == NODE_ENTER || node->type == BROKEN_SENSOR) {
        return node->edge[DIR_AHEAD].dest;
    } else if (node->type == NODE_BRANCH) {
        if (switch_state[switchIndex(node->num)] == 'C') {
            return node->edge[DIR_CURVED].dest;
        } else if (switch_state[switchIndex(node->num)] == 'S') {
            return node->edge[DIR_STRAIGHT].dest;
        } else {
            PANIC("switch direction needed");
        }
    } else if (node->type == NODE_EXIT) {
        return node;
    } else {
        logf("unknown next node");
        logf("unknown next node %x", node);
        logf("unknown next node: %d", node->type);
        logf("unknown next node: %s", node->name);
        PANIC("unknown next node 1");
    }
}

int distanceToNextNode(track_node* node) {
    if (node->type == NODE_SENSOR || node->type == NODE_MERGE || node->type == NODE_ENTER || node->type == BROKEN_SENSOR) {
        return node->edge[DIR_AHEAD].dist;
    } else if (node->type == NODE_BRANCH) {
        if (switch_state[switchIndex(node->num)] == 'C') {
            return node->edge[DIR_CURVED].dist;
        } else if (switch_state[switchIndex(node->num)] == 'S') {
            return node->edge[DIR_STRAIGHT].dist;
        } else {
            PANIC("switch direction needed");
        }
    } else if (node->type == NODE_EXIT) {
        PANIC("exit has no distance to next node");
    } else {
        PANIC("unknown next node 2");
    }
}

int distance(track_node* start, track_node* end) {

    ASSERT(start, "null pointer passed to distance start");
    ASSERT(start, "null pointer passed to distance end");

    track_node* node = start;

    int ret = 0;

    if (start == end) {
        return 0;
    }

    for (int i=0;;i++) {

        // todo: this used to be an assert
        if (i > 10000) return -1;

        track_node* next = findNextNode(node);
        ret += distanceToNextNode(node);

        if (next == end) return ret;
        node = next;
    }
}

track_node* findNextSensor(track_node* input_node) {

    ASSERT(input_node, "null pointer passed to findNextSensor");

    track_node* node = input_node;

    while (1) {
        if (node->type == NODE_SENSOR && node != input_node) {
            return node;
        } else if (node->type == NODE_EXIT) {
            return node;
        } else {
            node = findNextNode(node);
        }
    }
}

void adjustAndFw() {
    if (NUM_NODES == TRACK_MAX) {
        logf("we are in track A");
        logf("reminder: ensure BR3 is set to curved");
        // we are in track a
        trackNodeFromName("BR155")->edge[DIR_CURVED].dist = INF; // 155 must be set straight
        trackNodeFromName("BR156")->edge[DIR_STRAIGHT].dist = INF; // 156 must be set curved
        trackNodeFromName("BR3")->edge[DIR_STRAIGHT].dist = INF; // JUST DONT USE THIS CRAP
        trackNodeFromName("BR3")->edge[DIR_CURVED].dist = INF; // JUST DONT USE THIS CRAP
    } else if (NUM_NODES != TRACK_MAX) {
        logf("we are in track B");
        // we are in track B
        trackNodeFromName("E13")->type = BROKEN_SENSOR;
        trackNodeFromName("E14")->type = BROKEN_SENSOR;
        trackNodeFromName("C9")->type = BROKEN_SENSOR;
        trackNodeFromName("C10")->type = BROKEN_SENSOR;
        trackNodeFromName("BR155")->edge[DIR_STRAIGHT].dist = INF; // 155 must be set curved
        trackNodeFromName("BR156")->edge[DIR_CURVED].dist = INF; // 156 must be set straight
    }

    // populate graph[][]
    for (int i=0; i<TRACK_MAX; i++) {
        for (int j=0; j<TRACK_MAX; j++) {
            if (i == j) graph[i][j] = 0;
            else graph[i][j] = INF;
        }
    }
    for (int i=0; i<144; i++) {
        if (nodes[i].type == NODE_SENSOR || nodes[i].type == BROKEN_SENSOR || nodes[i].type == NODE_MERGE) {
            graph[i][nodes[i].edge[DIR_AHEAD].dest->id] = 1;
        } else if (nodes[i].type == NODE_BRANCH) {
            if (nodes[i].edge[DIR_STRAIGHT].dist < INF) graph[i][nodes[i].edge[DIR_STRAIGHT].dest->id] = 1;
            if (nodes[i].edge[DIR_CURVED].dist < INF) graph[i][nodes[i].edge[DIR_CURVED].dest->id] = 1;
        } else {
            // todo: enumerate?
            continue;
        }
    }

    FW_initializePathAndCost(graph);

    if (NUM_NODES == TRACK_MAX) {
        // track A
        track_node* start = trackNodeFromName("E14");
        track_node* end = trackNodeFromName("C16");
        ASSERT(INF > FW_getCost(start->id, end->id), "path should exist");
    } else {
        // track B
        track_node* start = trackNodeFromName("B2");
        track_node* end = trackNodeFromName("A3");
        ASSERT(FW_getCost(start->id, end->id) > 0, "should be possible");
    }
}

void initAndAdjustA() {
    init_tracka(nodes);
    adjustAndFw();
}

void initAndAdjustB() {
    init_trackb(nodes);
    adjustAndFw();
}
