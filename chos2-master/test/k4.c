#include <nameServer.h>
#include <clockServer.h>
#include <uart1.h>
#include <uart2.h>
#include <io.h>
#include <panic.h>
#include <parse.h>
#include <trackdata.h>
#include <trainServer2.h>
#include <uiLockServer.h>
#include <measureInner.h>
#include <pathfinding.h>
#include <switchServer.h>
#include <syslib.h>
#include <pathServer.h>
#include <idlePrinter.h>
#include <cbServer.h>
#include <cli.h>

void uptimeDisplay();
void sensorRequestor();

extern track_node nodes[TRACK_MAX];

void k4_main() {

    initSwitchServer();

    Create(-10, nameServer);
    Create(-9, clockServer);
    Create(-8, clockNotifier);
    Create(-7, lockServer);

    Create(1, com1RxServer);
    Create(2, com1RxNotifier);

    Create(2, com1TxServer);
    Create(3, com1TxNotifier);
    Create(4, com1MsNotifier);

    Create(1, com2RxNotifier);
    Create(2, com2TxServer);
    Create(3, com2TxNotifier);
    Create(4, com2RxServer);

    Create(5, cli);
    Create(5, uptimeDisplay);
    Create(5, sensorRequestor);

    Create(6, initAndAdjustA);
    Create(6, cbServer);
    Create(7, pathServer);

    Create(8, trainServer2);

    Create(100, idlePrinter);
}
