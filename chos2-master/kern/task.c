#include <ARM.h>
#include <task.h>
#include <syslib.h>
#include <bwio.h>


/*
    TrapFrameLayout:
    CPSR    <-- StackEntry
    R0      <-- SP stored here
    R1
    ...
    R15
*/
void initializeStack(Task* task, void* functionPtr){

    char* stack_base = task->STACK;
    int* stack = stack_base + STACK_SIZE;

    stack --;
    //PC
    *stack = (int) functionPtr;

    stack--;

    *stack = (int) Exit;

    //stack--;

    task->stackEntry =  (int*)((int)stack_base + STACK_SIZE) - 16;

    int i;
    // set r0-r12 registers to 0
    for(i=12;i>=0;i--){
        stack--;
        *stack = i;
    }

    stack --;

    int cpsr = USR_MODE;

    //cpsr status, for hardware interrupt capable trap frame
    *stack = cpsr;
}

void initializeTask(Task* task, int tId, int pId, int priority, taskStatus status, void* functionPtr){
    task->tId = tId;
    task->pId = pId;
    task->priority = priority;
    task->status = status;
    initializeStack(task, functionPtr);
}
