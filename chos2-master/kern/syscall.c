#include <ARM.h>
#include <syscode.h>
#include <kern.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <deviceRegistry.h>
#include <syscall.h>
#include <bwio.h>

extern Scheduler* scheduler;
extern COMM* com;
extern DeviceRegistry* registry;

void __attribute__((naked)) sys_handler(){
    asm("SUB SP, SP, #4");
    asm("STR LR, [SP]");
    asm("LDR R0, [LR, #-4]");

    asm("BL jumpTable");

    //jumps here to hand off suspendedTask resumptions
    asm("LDR R0, [SP]");
    asm("ADD SP, SP, #4");
    asm("BL handleSuspendedTasks");

    asm("LDR LR, =enterKernel");
    asm("MOV PC, LR");
}

//using R1, because these are less likely to be disturbed
static inline __attribute__((always_inline)) void enter_sys_mode() {
    asm("MSR CPSR_c, #"TOSTRING(SYS_MODE));
}

static inline __attribute__((always_inline)) void exit_sys_mode() {
    asm("MSR CPSR_c, #"TOSTRING(SVC_MODE));
}

void sysYield() { }

extern int kernelRunning;
void sysKillKernel() {
    bwprintf(COM2, "\r\nkilling kernel\r\n");
    kernelRunning = 0;
}

void sysGetTid(){
    register int* SP asm("r0");

    /*
    - switch to SYS_MODE
    - copy SP to the sp variable
    - switch to SVC_MODE
    */
    asm(R"(
        MSR CPSR_c, #0x9F
        MOV %0, SP
        MSR CPSR_c, #0x93
    )": "=r"(SP));

    SP[1] = scheduler->currentTask->tId;
}

void sysGetPid(){
    register int* SP asm("r0");

    /*
    - switch to SYS_MODE
    - copy SP to the sp variable
    - switch to SVC_MODE
    */
    asm(R"(
        MSR CPSR_c, #0x9F
        MOV %0, SP
        MSR CPSR_c, #0x93
    )": "=r"(SP));

    SP[1] = scheduler->currentTask->pId;
}

void sysAwaitEvent() {
    register int* SP asm("r0");

    /*
    - switch to SYS_MODE
    - copy SP to the SP variable
    - pop the argument off the stack
    - switch to SVC_MODE
    */
    asm(R"(
        MSR CPSR_c, #0x9F
        MOV %0, SP
        ADD SP, SP, #4
        MSR CPSR_c, #0x93
    )": "=r"(SP));

    int eventId = SP[0];

    Task* task = scheduler->currentTask;
    WaitForInterrupt(registry, task, eventId);
    task->status = BLOCKED;
}


void sysCreateTask(){
    void* funcPtr;
    int priority;
    int* sp;

    enter_sys_mode();
    asm("ADD SP, SP, #8"); // pop two arguments
    asm("MOV R0, SP");
    exit_sys_mode();

    asm("MOV %0, R0": "=r"(sp));

    priority = (int) sp[-2];
    funcPtr = (void*) sp[-1];
    int pId = scheduler->currentTask->tId;
    int result = scheduleTask(scheduler, priority, pId, funcPtr);
    sp[1] = result;
}

void sysExit(){
    scheduler->currentTask->status = EXITED;
}

void sysDestroy(){
    scheduler->currentTask->status = EXITED;
    freeTask(scheduler, scheduler->currentTask);
}

void sysSend(){
    int* sp;
    enter_sys_mode();
    asm("ADD SP, SP, #4"); // pop one argument - pointer to arguments array
    asm("MOV R0, SP");
    exit_sys_mode();

    asm("MOV %0, R0" : "=r"(sp));
    scheduler->currentTask->stackEntry = sp;

    int* args = (int*) sp[-1];

    int tid = args[0];
    void* msg = (void*) args[1];
    int msglen = args[2];
    void* rep = (void*) args[3];
    int replylen = args[4];

    int result = insertSender(com, scheduler->currentTask->tId, tid, msg, msglen, rep, replylen);
    if (result < 0) {
        sp[1] = result;
    } else {
        scheduler->currentTask->status = BLOCKED;
    }
}

void sysReceive(){
    int* sp;

    enter_sys_mode();
    asm("ADD SP, SP, #4"); // pop one argument - pointer to arguments array
    asm("MOV R0, SP");
    exit_sys_mode();

    asm("MOV %0, R0" : "=r"(sp));
    scheduler->currentTask->stackEntry = sp;
    int* args = (int*) sp[-1];
    char* msg = (char*) args[1];
    int msglen = args[2];
    insertReceiver(com, scheduler->currentTask->tId, msg, msglen);
    sp[3] = args[0];
}

void sysReply(){
    int* sp;
    enter_sys_mode();

    asm("ADD SP, SP, #4"); // pop one argument - pointer to arguments array
    asm("MOV R0, SP");

    exit_sys_mode();

    asm("MOV %0, R0" : "=r"(sp));

    int* args = (int*) sp[-1];
    int tid = (int) args[0];
    void* msg = (void*) args[1];
    int msglen = args[2];
    // bwprintf(COM2, "Replying %d, %x, %d", tid, msg, msglen);
    int result = reply(com, msg, msglen, tid);
    sp[1] = result;
}

//handles the switch statements
void jumpTable(int code){
    code &= SWI_OPCODE_FLAG;
    switch (code){
        case YIELD_CODE:
            sysYield();
            break;
        case CREATE_CODE:
            sysCreateTask();
            break;
        case MYTID_CODE:
            sysGetTid();
            break;
        case MYPARENTTID_CODE:
            sysGetPid();
            break;
        case EXIT_CODE:
            sysExit();
            break;
        case DESTROY_CODE:
            sysDestroy();
            break;
        case SEND_CODE:
            sysSend();
            break;
        case RECEIVE_CODE:
            sysReceive();
            break;
        case REPLY_CODE:
            sysReply();
            break;
        case AWAITEVENT_CODE:
            sysAwaitEvent();
            break;
        case KILL_KERNEL_CODE:
            sysKillKernel();
            break;
        default:
            bwprintf(COM2, "Unknown SWI code %d!\r\n", code);
            while(1){}
    }
}

void setUpSWIHandler(void* handle_swi) {
    /*
    Install SWI handler
    The swi instruction executes at address 0x8. The following two lines write
    this to the memory addresses:

    0x08        LDR pc, [pc, #0x18]
    0x0c        ?
    ...         ?
    0x28        <absolute address of handle_swi>

    Note that ARM prefetches 2 instructions ahead. Hence, after a software
    interrupt, instruction 0x08 executes with pc=0x10.
    */
    *((unsigned*)0x8) = 0xe59ff018;
    *((unsigned*)0x28) = (unsigned) handle_swi;
}
