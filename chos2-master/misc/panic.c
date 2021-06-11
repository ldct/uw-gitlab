#include <bwio.h>
#include <interrupt.h>
#include <timer.h>

void __attribute__((noreturn)) die() {
    bwprintf(COM1, "%c", 0x20);

    int now = getValue(3);
    while (now - getValue(3) < 508000) {}
    bwprintf(COM1, "%c%c", 0, 58);
    while (now - getValue(3) < 2*508000) {}
    bwprintf(COM1, "%c%c", 0, 74);
    while (now - getValue(3) < 3*508000) {}
    bwprintf(COM1, "%c%c", 0, 1);
    while (now - getValue(3) < 4*508000) {}
    bwprintf(COM1, "%c%c", 0, 24);
    while (now - getValue(3) < 5*508000) {}
    bwprintf(COM1, "%c%c", 0, 78);
    while (now - getValue(3) < 6*508000) {}
    bwprintf(COM1, "%c%c", 0, 79);
    for (;;) {}
}

void __attribute__((noreturn)) PANIC(const char* s) {
    setEnabledDevices(0x0, 0x0);
    bwprintf(COM2, "PANIC: %s \r\n", s);
    die();
}

void __attribute__((noreturn)) aFailed(char *file, int line, const char* msg) {
    setEnabledDevices(0x0, 0x0);
    bwprintf(COM2, "ASSERTION FAILURE %s at %s %d\r\n", msg, file, line);
    die();
}
