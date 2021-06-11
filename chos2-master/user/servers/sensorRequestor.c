#include <io.h>
#include <hud.h>
#include <ipc_codes.h>
#include <uiLockServer.h>
#include <cbServer.h>
#include <timer.h>
#include <syslib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <nameServer.h>
#include <panic.h>
#include <string.h>

const char* SENSOR_LINE_PREFIX = "Sensor - ";

char sensorPairCharacter(int sensors_state[80], int i) {
    ASSERT(0 <= i, "not a valid id");
    ASSERT(i < 40, "not a valid id");

    int left = sensors_state[2*i];
    int right = sensors_state[2*i + 1];

    if (left && right) {
        return 'x';
    } else if (left) {
        return '<';
    } else if (right) {
        return '>';
    } else {
        return '_';
    }
}

void computeSensorState(char com1_input[10], int sensors_state[80]) {
    for (int i=0;i<10;i++) {
        for (int j=0;j<8;j++) {
            int sensor_id = i*8 + j;
            sensors_state[sensor_id] = com1_input[i] & (1 << (7 - j));
        }
    }
}

// 13 -> 12; 1% load
void printSensorReport(char com1_input[10]) {
    int sensors_state[80];
    computeSensorState(com1_input, sensors_state);

    acquireUiLock();
    uprintf(COM2, "\033[s\033[%d;1H%s", SENSOR_LINENUM, SENSOR_LINE_PREFIX);
    for (int i=0; i<40; i++) {
        Putc(COM2, sensorPairCharacter(sensors_state, i));
    }
    Puts(COM2, "\033[u");
    releaseUiLock();
}

void renderSensorReport(char old[10], char new[10]) {
    int old_ss[80];
    int new_ss[80];
    computeSensorState(old, old_ss);
    computeSensorState(new, new_ss);
    
    acquireUiLock();
    uprintf(COM2, "\033[s");
    for (int i=0; i<40; i++) {
        if (
            sensorPairCharacter(new_ss, i) !=
            sensorPairCharacter(old_ss, i)
        ) {
            uprintf(
                COM2,
                "\033[%d;%dH%c", 
                SENSOR_LINENUM, 
                1 + i + strlen(SENSOR_LINE_PREFIX),
                sensorPairCharacter(new_ss, i)
            );
        }
    }
    uprintf(COM2, "\033[u");
    releaseUiLock();
}

void onSensorReport(char com1_input[10]) {
    int sensors_state[80];
    int i, j;
    for (i=0;i<10;i++) {
        for (j=0;j<8;j++) {
            int sensor_id = i*8 + j;
            sensors_state[sensor_id] = com1_input[i] & (1 << (7 - j));
        }
    }
    for (i=0; i<80; i++) {
        if (sensors_state[i]) {
            // lprintf(COM2, "triggered %d\r\n", i);
            FireCb(i);
        }
    }
}

void sensorRequestor() {
    char old_com1_input[10];
    char com1_input[10];
    char diff_input[10];

    unsigned lastTime = 0;
    unsigned thisTime = 0;

    for (int i=0; i<10; i++) {
        old_com1_input[i] = 0;
    }

    printSensorReport(old_com1_input);

    for (int i=0;; i=1) {
        thisTime = getValue(3);
        if (lastTime != 0) {
            // 5% load
            lprintf(COM2, "\033[s\033[%d;1HSensor Delay - %u ms (+ %u)                    \033[u", FPS_LINENUM, (lastTime - thisTime) / 508, (lastTime - thisTime) % 508);
        }
        lastTime = thisTime;
        Putc(COM1, 0x85);
        Delay(WhoIs("cs"), 10);
        for (int i=0; i<10; i++) {
            com1_input[i] = Getc(COM1);
        }

        v_Send_c(WhoIs("com1TxS"), UART1_SENSOR_REPORT);

        if (i == 0) continue;

        for (int i=0; i<10; i++) {
            diff_input[i] = ~old_com1_input[i] & com1_input[i];
        }

        renderSensorReport(old_com1_input, com1_input);
        onSensorReport(diff_input);

        for (int i=0; i<10; i++) {
            old_com1_input[i] = com1_input[i];
        }
    }
}
