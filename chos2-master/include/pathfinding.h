#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <tracknode.h>

void initAndAdjustA();
void initAndAdjustB();
track_node* trackNodeFromId(int id);
track_node* trackNodeFromName(const char* name);
track_node* findNextNode(track_node* node);
track_node* findNextSensor(track_node* input_node);
int distance(track_node* start, track_node* end);
track_node* sensorNodeFromAddress(int sensorAddress);

#endif
