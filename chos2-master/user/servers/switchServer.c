#include <io.h>
#include <marklin.h>
#include <switchServer.h>
#include <panic.h>
#include <ipc_codes.h>
#include <uiLockServer.h>
#include <hud.h>

char switch_state[NUM_SWITCHES];

void initSwitchServer() {
    for (int i=0; i<NUM_SWITCHES; i++) {
        switch_state[i] = '?';
    }
}

int switch_address(int idx) {
	if (idx < 18) return idx + 1;
	if (idx == 18) return 0x9A;
	if (idx == 19) return 0x9B;
	if (idx == 20) return 0x9C;
	if (idx == 21) return 0x99;
    PANIC("switch_address");
}

void setSwitchSlab(int state[NUM_SWITCHES]) {
    for (int i=0; i<NUM_SWITCHES; i++) {
        setTurnout(switch_address(i), state[i]);
    }
}

int switchIndex(int address) {
    ASSERT(1 <= address, "switch address out of range");
    if (address <= 18) {
        return address -1;
    }

    if (address == 0x9A) return 18;
	if (address == 0x9B) return 19;
	if (address == 0x9C) return 20;
	if (address == 0x99) return 21;

    PANIC("switch address out of range");
}

void allSwitchesTo(char pos) {
    char i;
    for (i=1; i<=18; i++) {
        setTurnout(i, pos);
    }
    for (i=0x9A; i<=0x9C; i++) {
        setTurnout(i, pos);
    }
    for (i=0x99; i<=0x99; i++) {
        setTurnout(i, pos);
    }
    // Sei vorsichtig!!!!!!111
    // Mach die solenoiden auf
    Putc(COM1, 0x20);
}

void switchStraight() {
    allSwitchesTo(POS_STRAIGHT);
}

void switchCurved() {
    allSwitchesTo(POS_CURVED);
}

void setInitialSwitches() {
    // leave the high-address ones unset. We don't want to use the central portion for now.
    setTurnout(1, POS_CURVED);
    setTurnout(2, POS_CURVED);
    setTurnout(3, POS_CURVED);
    setTurnout(4, POS_STRAIGHT);
    setTurnout(5, POS_CURVED);
    setTurnout(6, POS_CURVED);
    setTurnout(7, POS_CURVED);
    setTurnout(8, POS_CURVED);
    setTurnout(9, POS_STRAIGHT);
    setTurnout(10, POS_STRAIGHT);
    setTurnout(11, POS_CURVED);
    setTurnout(12, POS_CURVED);
    setTurnout(13, POS_STRAIGHT);
    setTurnout(14, POS_STRAIGHT);
    setTurnout(15, POS_CURVED);
    setTurnout(16, POS_STRAIGHT);
    setTurnout(17, POS_STRAIGHT);
    setTurnout(18, POS_CURVED);

    // must be set like this on track A
    setTurnout(0x9B, POS_STRAIGHT);
    setTurnout(0x9C, POS_CURVED);

    setTurnout(0x9A, POS_CURVED);
    setTurnout(0x99, POS_CURVED);
    
    // Sei vorsichtig!!!!!!111
    // Mach die solenoiden auf
    Putc(COM1, 0x20);
}

void printSwitchStatus() {
    acquireUiLock();
    uprintf(COM2, "\033[s\033[%d;1HSwitches - ", SWITCH_LINENUM);
    for (int i=0; i<NUM_SWITCHES; i++) {
        Putc(COM2, switch_state[i] == '?' ? '_' : switch_state[i]);
    }
    uprintf(COM2, "\033[u");
    releaseUiLock();
}

void setTurnout(int number, int position) {
    if (switch_state[switchIndex(number)] == 'C' && position == POS_CURVED) {
        return;
    }
    if (switch_state[switchIndex(number)] == 'S' && position == POS_STRAIGHT) {
        return;
    }
    Putc2(COM1, position, number);
    switch_state[switchIndex(number)] = (position == POS_CURVED) ? 'C' : 'S';
    printSwitchStatus();
}

void setTurnoutNB(int number, int position) {
    if (switch_state[switchIndex(number)] == 'C' && position == POS_CURVED) {
        return;
    }
    if (switch_state[switchIndex(number)] == 'S' && position == POS_STRAIGHT) {
        return;
    }
    Putc2NB(COM1, position, number);
    switch_state[switchIndex(number)] = (position == POS_CURVED) ? 'C' : 'S';
    printSwitchStatus();
}
