#ifndef MAP_H
#define MAP_H

#include <queue.h>

typedef struct _Node {
    int key;
    void* value;
    int height;
    struct _Node* left;
    struct _Node* right;
    struct _Node* up;
} Node;

typedef struct _Map {
    Node* root;
    Node nodes[QUEUE_SIZE];
    Queue freeQueue;
    void* retainer;
} Map;

void initializeMap(Map* map);

// returns -1 if not enough space
// no one reads the return value
int insertMap(Map* map, int key, void* value);

// returns 0 for updated, 1 for newly created
int putMap(Map* map, int key, void* value);

void* getMap(const Map* map, int key);

void* removeMap(Map* map, int key);

Node* iterateMap(Map* map, Node* key);

void printTree(Map* map);

#endif
