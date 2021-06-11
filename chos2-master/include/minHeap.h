#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE

#include <task.h>

/**
 * Fixed size, efficient, runs great
 **/

// Has to be 2**n - 1
#define MINHEAP_SIZE 255

typedef struct _KV {
    int key;
    void* value;
} KV;

typedef struct _MinHeap {
    int length;
    KV heap[MINHEAP_SIZE];
} MinHeap;

KV* peek(MinHeap* heap);

void initializeMinHeap(MinHeap* heap);

void* removeMinHeap(MinHeap* heap);

int insertMinHeap(MinHeap* heap, int key, void* value);

void printStackHeap(MinHeap* heap);
#endif
