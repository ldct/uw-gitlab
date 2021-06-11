#include <syslib.h>
#include <bwio.h>
#include <map.h>

void k1_child() {
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Yield();
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Exit();
}

void k1_main(){
    int ret = Create(-1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(-1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    bwprintf(COM2, "FirstUserTask: Exit\r\n");
}
