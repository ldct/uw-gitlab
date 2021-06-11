#ifndef SYSCODE_H
#define SYSCODE_H

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define YIELD_CODE 0
#define CREATE_CODE 1
#define MYTID_CODE 2
#define MYPARENTTID_CODE 3
#define EXIT_CODE 4
#define DESTROY_CODE 5
#define SEND_CODE 6
#define RECEIVE_CODE 7
#define REPLY_CODE 8
#define AWAITEVENT_CODE 9
#define KILL_KERNEL_CODE 10

#endif
