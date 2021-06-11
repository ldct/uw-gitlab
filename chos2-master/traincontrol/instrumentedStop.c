#include <timer.h>
#include <io.h>
#include <trainnum.h>

void instrumentedStop(const char* procName, int trainNum) {
    ASSERT_IS_TRAINNUM(trainNum);

    int start = getValue(3);
    Putc2(COM1, 0, trainNum);
    logf("%s: halten Sie bitte (%d ms)", procName, (start - getValue(3))/508);
}
