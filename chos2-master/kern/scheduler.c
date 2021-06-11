#include <ARM.h>
#include <kern.h>
#include <stdlib.h>
#include <syscode.h>
#include <scheduler.h>
#include <bwio.h>
#include <panic.h>

extern Scheduler* scheduler;

int taskID;

void printRegisters(int* stack){
    bwprintf(COM2, "Stack pointer : %x\r\n", stack);
    bwprintf(COM2, "CPSR [%x]: %x\r\n", stack, *stack);
    stack++;
    int i=0;
    for(i=0;i<16;i++){
        bwprintf(COM2, "R%d [%x]: %x\r\n", i, stack, *stack);
        stack++;
    }
}

void printTask(Task* task){
    bwprintf(COM2, "Task Address: %x\r\n", task);
    bwprintf(COM2, "Entering Task: %d\r\n", task->tId);
    printRegisters(task->stackEntry);
}

void initializeScheduler(Scheduler* scheduler){
    taskID = 1;
    intializePriorityQueue(&(scheduler->readyQueue));
    initializeQueue("taskFQ", &(scheduler->freeQueue));
    initializeMap(&(scheduler->taskTable));
    int i;
    for(i=0;i<MAX_TASKS;i++){
	    push(&(scheduler->freeQueue), scheduler->tasks + i);
    }
    scheduler->currentTask = NULL;
}

//Allocates, configures and setups the stack layout for context switch
int scheduleTask(Scheduler* scheduler, int priority, int parent, void* functionPtr){
    Task* task = pop(&(scheduler->freeQueue));
    if (!task){
        return -2;
    }
    int tId = taskID++;
    initializeTask(task, tId, parent, priority, READY, functionPtr);
    insertMap(&(scheduler->taskTable), tId, task);
    insertTaskToQueue(scheduler, task);
    return tId;
}

void freeTask(Scheduler* scheduler, Task* task){
    push(&(scheduler->freeQueue), task);
}

int runFirstAvailableTask(Scheduler* scheduler) {
    Task* task;
    task = removeMin(&(scheduler->readyQueue));
    // bwprintf(COM2, "Removing task %x\r\n", task);
    if(task){
        runTask(scheduler, task);
        return 0;
    } else{
        return -1;
    }
}

void runTask(Scheduler* scheduler, Task* task){
    scheduler->currentTask = task;
    task->status = RUNNING;
    exitKernel(task->stackEntry);
}

int insertTaskToQueue(Scheduler* scheduler, Task* task){
    task->status = READY;
    // bwprintf(COM2, "Fuck 452 %x \r\n", task);
    return insert(&(scheduler->readyQueue), task);
}

//TODO: Delete entries from taskTable when they are destroyed
Task* getTask(Scheduler* schedule, int tId){
    return getMap(&(schedule->taskTable), tId);
}


void handleSuspendedTasks(void* lr){
    // bwprintf(COM2, "Handling the fucking task\r\n");
    int* stack;
    //changes from svc to sys mode
    asm(R"(
        MSR CPSR_c, #0x9F
        MOV R3, SP
        MSR CPSR_c, #0x93
        MOV %[stack], R3
    )" : [stack]"=r"(stack)::"r3");

    stack[15] = (int) lr;
    scheduler->currentTask->stackEntry = stack;
    if (scheduler->currentTask->status == RUNNING) {
        // todo: this failed once
        int code = insertTaskToQueue(scheduler, scheduler->currentTask);
    }
    // printTask(scheduler->currentTask);
    scheduler->currentTask = NULL;
}
