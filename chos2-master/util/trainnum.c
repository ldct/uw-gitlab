#include <trainnum.h>
#include <panic.h>

int isTrainnum(int trainNum) {
    return (
        trainNum == 1 ||
        trainNum == 24 ||
        trainNum == 58 ||
        trainNum == 74 ||
        trainNum == 78 ||
        trainNum == 79
    );
}

void ASSERT_IS_TRAINNUM(int trainNum) {
    ASSERT(
        isTrainnum(trainNum),
        "not a trainNum"
    );
}
