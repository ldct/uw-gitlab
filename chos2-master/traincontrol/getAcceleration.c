#include <getAcceleration.h>
#include <panic.h>

int getAcceleration(int trainNum) {
    if (trainNum == 1) return 136;
    if (trainNum == 24) return 147;
    if (trainNum == 58) return 126;
    if (trainNum == 74) return 196; // note: this is for high speed
    if (trainNum == 78) return 118;
    if (trainNum == 79) return 156;
    PANIC("not a trainNum");
}
