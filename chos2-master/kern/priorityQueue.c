#include <stdlib.h>
#include <priorityQueue.h>
#include <bwio.h>
#include <panic.h>

void intializePriorityQueue(PriorityQueue* queue){
    queue->length = 0;
}

void reposition(PriorityQueue* queue, int index){
    Task* task = queue->queue[index];
    Task* temp;
    int direction;
    int target = (index - 1)/2;
    if(target>=0 && task->priority<queue->queue[target]->priority){
        direction  = -1;
        temp = queue->queue[target];
        queue->queue[target] = task;
        queue->queue[index] = temp;
        index = target;
    } else {
        direction = 1;
    }
    for(;;){
        if(direction>0){
            target = 2*index;
            if(target+1>=queue->length){
                return;
            } else if(target+2==queue->length) {
		target += 1;
	    }
	    else {
            target += (queue->queue[target+1]->priority < queue->queue[target+2]->priority)? 1 : 2;
	    }
            if(task->priority <= queue->queue[target]->priority){
                return;
            }
        } else{
            target = (index - 1)/2;
            if(target < 0){
                return;
            }
            if(task->priority>=queue->queue[target]->priority){
                return;
            }
        }
        temp = queue->queue[target];
        queue->queue[target] = task;
        queue->queue[index] = temp;
        index = target;
    }
}

Task* removeMin(PriorityQueue* queue){
    if(queue->length==0){
        return NULL;
    }
    Task* result = queue->queue[0];
    queue->queue[0] = queue->queue[--queue->length];
    reposition(queue, 0);
    return result;
}

int insert(PriorityQueue* queue, Task* payload){
    if(queue->length+1 == P_QUEUE_SIZE){
        PANIC("pqueue too big");
    }
    queue->queue[queue->length++] = payload;
    reposition(queue, queue->length - 1);
    return 0;
}

void printStack(PriorityQueue* queue){
    int i=0;
    int upgrade = 1;
    for(i=0;i<queue->length;i++){
        if(i==upgrade){
           bwprintf(COM2, "\r\n");
           upgrade*= 2;
        }
        bwprintf(COM2, " %d ", queue->queue[i]);
    }
}
