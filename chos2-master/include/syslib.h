/*
    Library exposing syscall functionalities to user programs
*/

int Create(int priority, void (*function)());

int AwaitEvent(int eventId);

int MyTid();

int MyParentTid();

void Yield();

void Exit();

void Destroy();

int Send(int tid, const void* msg, int msglen, void* reply, int replylen);

void Receive(int* tid, void* msg, int msglen);

int Reply(int tid, const void* reply, int replylen);

void KillKernel();

// Wrappers

int Reply0(int tid);

void v_Reply_c(int tid, char r);

void v_Reply_i(int tid, int val);

void v_Send_i(int tid, int x);

void v_Send_c(int tid, char c);

char c_Send_c(int tid, char c);

int i_Send_c(int tid, char c);

int i_Send_cc(int tid, char c0, char c1);

void v_Send_cc(int tid, char c0, char c1);

void v_Send_ccc(int tid, char c0, char c1, char c2);

char c_Receive_v();

void v_Send_ii(int tid, int i0, int i1);

void v_Send_ci(int tid, char c, int i);

void c400_Send_ccii(int tid, char c0, char c1, int i0, int i1, int out[100]);
