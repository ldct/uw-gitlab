#include <speedRegime.h>
#include <panic.h>
#include <trainnum.h>
#include <getTravelVset.h>

int getTravelVset(int trainNum) {
    ASSERT_IS_TRAINNUM(trainNum);
    if (SPEEDREGIME_HIGH) return 14;

    if (trainNum == 1) return 9;
    if (trainNum == 24) return 9;
    if (trainNum == 58) return 10;
    if (trainNum == 74) return 7;
    if (trainNum == 78) return 9;
    if (trainNum == 79) return 8;

    PANIC("not a trainnum");
}
