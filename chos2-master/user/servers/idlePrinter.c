#include <io.h>
#include <clockServer.h>
#include <nameServer.h>
#include <hud.h>

extern int utilTimePct;

void idlePrinter() {
    int csTid = WhoIs("cs");
    while (1) {
        if (0 <= utilTimePct && utilTimePct <= 1000) {
            lprintf(COM2,
                "\033[s\033[%d;1HUtiltime - %d.%d%%  \033[u",
                UTILTIME_LINENUM,
                utilTimePct / 10,
                utilTimePct % 10
            );
        }
        Delay(csTid, 100);
    }
}
