#include <stdlib.h>
#include <minHeap.h>
#include <bwio.h>

void initializeMinHeap(MinHeap* heap){
    heap->length = 0;
}

KV* peek(MinHeap* heap){
    if(heap->length)
	return (KV*)heap->heap;
    else
	return NULL;
}

void repositionHeap(MinHeap* heap, int index){
    int key = heap->heap[index].key;
    int temp_key;
    void* temp_value;
    int direction;
    int target = (index - 1)/2;
    if(target>=0 && key<heap->heap[target].key){
        direction  = -1;
        temp_key = heap->heap[target].key;
        temp_value = heap->heap[target].value;
        heap->heap[target].key = key;
        heap->heap[target].value = heap->heap[index].value;
        heap->heap[index].key = temp_key;
        heap->heap[index].value = temp_value;
        index = target;
    } else {
        direction = 1;
    }
    for(;;){
        key = heap->heap[index].key;
        if(direction>0){
            target = 2*index;
            if(target+1>=heap->length){
		        return;
	        } else if(target+2==heap->length){
                target = target+1;
            } else if(heap->heap[target+2].key<heap->heap[target+1].key){
                target = target+2;
            } else {
                target = target+1;
            }
            if(key <= heap->heap[target].key){
                return;
            }
        } else{
            target = (index - 1)/2;
            if(target < 0){
                return;
            }
            if(key>=heap->heap[target].key){
                return;
            }
        }
        temp_key = heap->heap[target].key;
        temp_value = heap->heap[target].value;
        heap->heap[target].key = key;
        heap->heap[target].value = heap->heap[index].value;
        heap->heap[index].key = temp_key;
        heap->heap[index].value = temp_value;
        index = target;
    }
}

void* removeMinHeap(MinHeap* heap){
    if(heap->length==0){
        return NULL;
    }
    void* result = heap->heap[0].value;
    heap->heap[0].key = heap->heap[--heap->length].key;
    heap->heap[0].value = heap->heap[heap->length].value;
    repositionHeap(heap, 0);
    return result;
}

int insertMinHeap(MinHeap* heap, int key, void* value){
    if(heap->length+1 == MINHEAP_SIZE){
        return -1;
    }
    heap->heap[heap->length].key = key;
    heap->heap[heap->length].value = value;
    repositionHeap(heap, heap->length++);
    return 0;
}

void printStackHeap(MinHeap* heap){
    int i=0;
    for(i=0;i<heap->length;i++){
        bwprintf(COM2, " %d ", heap->heap[i].key);
    }
}
