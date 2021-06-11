#include <getMinLmVset.h>
#include <panic.h>

int getMinLmVset(int trainNum) {
    if (trainNum == 1) return 8;
    if (trainNum == 24) return 8;
    if (trainNum == 58) return 8;
    if (trainNum == 74) return 5;
    if (trainNum == 78) return 9;
    if (trainNum == 79) return 1;
    PANIC("not a trainNum");
}
