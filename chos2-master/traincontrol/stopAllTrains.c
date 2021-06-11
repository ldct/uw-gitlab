#include <io.h>

void stopAllTrains() {
    Putc2(COM1, 0, 58);
    Putc2(COM1, 0, 78);
    Putc2(COM1, 0, 74);
    Putc2(COM1, 0, 1);
    Putc2(COM1, 0, 24);
}
