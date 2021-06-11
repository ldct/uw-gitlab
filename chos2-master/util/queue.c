#include <queue.h>
#include <bwio.h>
#include <panic.h>

int ring(int num) {
    return num % QUEUE_SIZE;
}

int ringFill(Queue* queue) {
    return queue->length - queue->cursor;
}

int ringCapacity(Queue* queue) {
    return QUEUE_SIZE - (ringFill(queue));
}

void initializeQueue(const char* name, Queue* queue) {
    queue->cursor = 0;
    queue->length = 0;
    queue->name = name;
}

void* pop(Queue* queue) {
    if (ringFill(queue)) {
        void* ret = queue->queue[ring(queue->cursor++)];
        return ret;
    } else {
        return 0;
    }
};

void* peep(Queue* queue) {
    if (ringFill(queue)) {
        void* ret = queue->queue[ring(queue->cursor)];
        return ret;
    } else {
        return 0;
    }
};

void push(Queue* queue, void* obj) {
    if (!ringCapacity(queue)) {
        PANIC("queue capacity reached");
    }
    // ASSERT(ringCapacity(queue), "queue capacity reached");
    queue->queue[ring(queue->length++)] = obj;
}
