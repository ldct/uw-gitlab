#ifndef PATHSERVER_H
#define PATHSERVER_H

void pathServer();

int pathExists(track_node* start, track_node* end);
void findPath(char trainNum, track_node* start, track_node* end, int out[100]);
void findPathR(char trainNum, track_node* start, track_node* end, int out[100]);
void releasePath(char trainNum);

#endif
