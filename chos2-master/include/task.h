#ifndef TASK_H
#define TASK_H

// this can't be changed
#define STACK_SIZE 65536

typedef enum task_Status{
    RUNNING,
    READY,
    BLOCKED,
    EXITED,
    HALTED
} taskStatus;

typedef struct _Task {
    int pId; // parentId
    int tId; // taskId
    int priority;

    char STACK[STACK_SIZE];

    // current sp
    int* stackEntry;

    taskStatus status;
} Task;

void initializeTask(Task* task, int tId, int pId, int priority, taskStatus status, void* functionPtr);

#endif
