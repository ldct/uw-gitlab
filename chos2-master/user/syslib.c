#include <syscode.h>
#include <syslib.h>
#include <bwio.h>
#include <scheduler.h>
#include <panic.h>

#define MAX_ARGS 10

static inline __attribute__((always_inline)) void save_user_context() {
    asm("STMFD SP!, {R0-R12, R14-R15}");
    asm("MRS R2, CPSR");
    asm("STMFD SP!, {R2}");
}

int Create(int priority, void (*function)()) {
    save_user_context();
    //argument stuffing now
    asm("STMFD SP!, {R0-R1}");

    asm("SWI " TOSTRING(CREATE_CODE) ::: "r0");
    int ret;
    asm("MOV %0, R0" : "=r" (ret));
    return ret;
}

int AwaitEvent(int eventId) {
    save_user_context();
    //argument stuffing
    asm("STMFD SP!, {R0}");

    asm("SWI " TOSTRING(AWAITEVENT_CODE) ::: "r0");
    return 0;
}

int MyTid(){
    save_user_context();
    asm("SWI " TOSTRING(MYTID_CODE) ::: "r0");
    int ret;
    asm("MOV %0, R0" : "=r" (ret));
    return ret;
}

int MyParentTid(){
    save_user_context();
    asm("SWI " TOSTRING(MYPARENTTID_CODE) ::: "r0");
    int ret;
    asm("MOV %0, R0" : "=r" (ret));
    return ret;
}

void Yield() {
    save_user_context();
    asm("SWI " TOSTRING(YIELD_CODE));
}

void KillKernel() {
    save_user_context();
    asm("SWI " TOSTRING(KILL_KERNEL_CODE));
}

void Exit() {
    save_user_context();
    asm("SWI " TOSTRING(EXIT_CODE));
}

void Destroy() {
    save_user_context();
    asm("SWI " TOSTRING(DESTROY_CODE));
}

typedef struct _Arguments {
    int args[MAX_ARGS];
} Arguments;

void variadicStore(Arguments* argv, int argc, ...){
    va_list varags;
    va_start(varags, argc);
    int i;
    for(i=0;i<argc;i++){
        argv->args[i] = va_arg(varags, int);
    }
    va_end(varags);
}

#pragma GCC diagnostic ignored "-Wreturn-type"
int Send(int tid, const void* msg, int msglen, void* reply, int replylen) {
    ASSERT(tid != -1, "cannot send to tid=-1");
    Arguments argument;
    variadicStore(&argument, 5, tid, msg, msglen, reply, replylen);
    save_user_context();
    asm("SUB SP, SP, #4");
    asm("STR %0, [SP]"::"r"(argument.args));
    asm("SWI " TOSTRING(SEND_CODE));
}

void Receive(int* tid, void* msg, int msglen) {
    Arguments argument;
    variadicStore(&argument, 3, tid, msg, msglen);
    save_user_context();

    asm("SUB SP, SP, #4");
    asm("STR %0, [SP]"::"r"(argument.args));
    asm("SWI " TOSTRING(RECEIVE_CODE));

    //Pro gamer move:
    //pushes the sender into tid, stored at R2
    asm("STR R1, [R2]");
}

int Reply(int tid, const void* reply, int replylen) {
    Arguments argument;
    variadicStore(&argument, 3, tid, reply, replylen);
    save_user_context();
    asm("SUB SP, SP, #4");
    asm("STR %0, [SP]"::"r"(argument.args));
    asm("SWI " TOSTRING(REPLY_CODE));
}
#pragma GCC diagnostic pop

int Reply0(int tid) {
    return Reply(tid, 0x0, 0);
}

void v_Reply_c(int tid, char r) {
    Reply(tid, &r,  1);
}

void v_Reply_i(int tid, int val) {
    char reply[4];
    *(int*)reply = val;
    Reply(tid, reply,  4);
}

void v_Send_i(int tid, int x) {
    Send(tid, &x, 4, 0x0, 0);
}

void v_Send_c(int tid, char c) {
    Send(tid, &c, 1, 0x0, 0);
}

char c_Send_c(int tid, char c) {
    char replyBuf;
    Send(tid, &c, 1, &replyBuf, 1);
    return replyBuf;
}

int i_Send_c(int tid, char c) {
    int replyBuf;
    Send(tid, &c, 1, &replyBuf, 4);
    return replyBuf;
}

int i_Send_cc(int tid, char c0, char c1) {
    int replyBuf;
    char c[2];
    c[0] = c0;
    c[1] = c1;
    Send(tid, c, 2, &replyBuf, 4);
    return replyBuf;
}

void v_Send_cc(int tid, char c0, char c1) {
    char c[2];
    c[0] = c0;
    c[1] = c1;
    Send(tid, c, 2, 0x0, 0);
}

void v_Send_ccc(int tid, char c0, char c1, char c2) {
    char c[3];
    c[0] = c0;
    c[1] = c1;
    c[2] = c2;
    Send(tid, c, 3, 0x0, 0);
}

void v_Send_ci(int tid, char c, int i) {
    char buf[5];

    buf[0] = c;
    int* t = (int*) (buf+1);

    *t = i;
    Send(tid, buf, 5, 0x0, 0);
}

char c_Send_ci(int tid, char c, int i) {
    char reply;
    char buf[5];

    buf[0] = c;
    int* t = (int*) (buf+1);

    *t = i;
    Send(tid, buf, 5, &reply, 1);
    return reply;
}

void v_Send_ii(int tid, int i0, int i1) {
    char c[8];

    int* t0 = (int*) c;
    int* t1 = (int*) (c+4);

    *t0 = i0;
    *t1 = i1;
    Send(tid, c, 8, 0x0, 0);
}

void c400_Send_ccii(int tid, char c0, char c1, int i0, int i1, int out[100]) {
    char req[10];

    req[0] = c0;
    req[1] = c1;
    int* t0 = (int*) (req+2);
    int* t1 = (int*) (req+6);

    *t0 = i0;
    *t1 = i1;

    Send(tid, req, 10, out, 400);
}

char c_Receive_v() {
    int creator;
    char val;
    Receive(&creator, &val, 1);
    Reply0(creator);
    return val;
}
