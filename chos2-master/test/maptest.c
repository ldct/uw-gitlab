#include <map.h>
#include <stdlib.h>
#include <minHeap.h>
#include <bwio.h>

void MapTest(){
    Map map;
    Map* m = &map;
    initializeMap(m);
    insertMap(m, 1, NULL);
    printTree(m);
    insertMap(m, 2, NULL);
    printTree(m);
    insertMap(m, 3, NULL);
    printTree(m);
    insertMap(m, 4, NULL);
    printTree(m);
    insertMap(m, 5, NULL);
    printTree(m);
    insertMap(m, 6, NULL);
    printTree(m);
    insertMap(m, 7, NULL);
    printTree(m);
    insertMap(m, 8, NULL);
    printTree(m);
    insertMap(m, 9, NULL);
    printTree(m);
    removeMap(m, 23);
    printTree(m);
    removeMap(m, 25);
    insertMap(m, 35, NULL);
    insertMap(m, 6, NULL);
    insertMap(m, 2, NULL);
    insertMap(m, 5, NULL);
    insertMap(m, 43, NULL);
    insertMap(m, 62, NULL);
    removeMap(m, 63);
    removeMap(m, 35);
    printTree(m);
    insertMap(m, 15, NULL);
}

void MapTestPut(){
    Map map;
    Map* m = &map;
    initializeMap(m);
    putMap(m, 1, NULL);
    putMap(m, 2, NULL);
    putMap(m, 3, NULL);
    putMap(m, 4, NULL);
    putMap(m, 5, NULL);
    putMap(m, 6, NULL);
    putMap(m, -1, NULL);
    putMap(m, 2, NULL);
    putMap(m, 1, (void*) 2);
    putMap(m, 2, NULL);
    putMap(m, 2, NULL);
    bwputr(COM2, (int) getMap(m, 1));

}

void DynamoTest(){
    bwprintf(COM2, "Starting dynamo test\r\n");
    Map map;
    Map* m = &map;
    initializeMap(m);
    putMap(m, 4, NULL);
    putMap(m, 37, NULL);
    putMap(m, 95, NULL);
    putMap(m, 197, NULL);
    putMap(m, 56, NULL);
    putMap(m, 22, NULL);
    putMap(m, 117, NULL);
    putMap(m, 100011, NULL);
    putMap(m, 18, NULL);
    putMap(m, 78, NULL);
    putMap(m, 63, NULL);
    putMap(m, 15, NULL);
    putMap(m, 12, NULL);
    Node* node = NULL;
    printTree(m);
    do {
	    bwprintf(COM2, "Looping\r\n");
	    node = iterateMap(m, node);
	    if (node) bwprintf(COM2, "Element: %d at %x\r\n", node->key, node);
	    bwprintf(COM2, "Seriously, this value is %d\r\n", node);
    } while (node!=NULL);
}


void clockTest() {
    int i=0;
    for(;;){
        i++;
//        bwprintf(COM2, "Run\r\n");
    }
}

void heapTest(){
    MinHeap heap;
    initializeMinHeap(&heap);
    insertMinHeap(&heap, 6, 0);
    insertMinHeap(&heap, 3, 0);
    insertMinHeap(&heap, 4, 0);
    insertMinHeap(&heap, 1, 0);
    insertMinHeap(&heap, 72, 0);
    insertMinHeap(&heap, 47, 0);
    insertMinHeap(&heap, 56, 0);
    insertMinHeap(&heap, 5, 0);
    insertMinHeap(&heap, 67, 0);
    insertMinHeap(&heap, 67, 0);
    insertMinHeap(&heap, 7, 0);
    insertMinHeap(&heap, 8, 0);
    printStackHeap(&heap);
}
