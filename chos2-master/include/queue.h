#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 64

typedef struct _Queue {
    const char* name;
    int cursor;
    int length;
    void* queue[QUEUE_SIZE];
} Queue;

void initializeQueue(const char* name, Queue* queue);

void* pop(Queue* queue);

void push(Queue* queue, void* obj);

int ringFill(Queue* queue);

int ring(int num);

#endif
