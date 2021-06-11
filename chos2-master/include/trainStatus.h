#ifndef TRAINSTATUS_H
#define TRAINSTATUS_H

#include <tracknode.h>
#include <queue.h>

#define INVSPEED_SAMPLES 12

typedef struct _TrainStatus {
    int trainNum;
    int vset;
    track_node* last_tripped;
    int last_tripped_time;
    track_node* prev_tripped;
    int prev_tripped_time;
    int is_reversing;
    Queue invspeeds;
} TrainStatus;

#endif
