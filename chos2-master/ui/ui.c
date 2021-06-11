#include <io.h>
#include <panic.h>
#include <marklin.h>
#include <timer.h>
#include <estimator.h>
#include <parse.h>
#include <ipc_codes.h>
#include <tracknode.h>
#include <graph.h>
#include <switchServer.h>
#include <trainData.h>
#include <cli.h>
#include <hud.h>
#include <syslib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <string.h>

#define UPTIME_PREFIX "Uptime - "

// 2% load
void uptimeDisplay() {

    lprintf(COM2, "\033[s\033[%d;1H%s\033[u", UPTIME_LINENUM, UPTIME_PREFIX);

    int csTid = WhoIs("cs");
    for (int i=0;;i++) {
        Delay(csTid, 10);
        lprintf(COM2,
            "\033[s\033[%d;%dH%d\033[u",
            UPTIME_LINENUM,
            1 + strlen(UPTIME_PREFIX),
            i
        );
    }
}
