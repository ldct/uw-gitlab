#include <tracknode.h>
#include <hud.h>
#include <io.h>
#include <panic.h>
#include <uiLockServer.h>
#include <string.h>

typedef struct _Coordinate {
    const char* name;
    int x;
    int y;
} Coordinate;

Coordinate coordinates[] = {
    // not shown: b14
    {"A3" , 3, 6},
    {"A4" , 3, 6},
    {"B15", 4, 6},
    {"B16", 4, 6},
    {"C9" , 5, 7},
    {"C10", 5, 7},
    {"B1" , 5, 8},
    {"B2" , 5, 8},
    {"D13", 5, 10},
    {"D14", 5, 10},
    {"E13", 5, 11},
    {"E14", 5, 11},
    {"E9" , 4, 12},
    {"E10", 4, 12},
    {"D5" , 3, 12},
    {"D6" , 3, 12},
    {"E5" , 2, 11},
    {"E6" , 2, 11},
    {"D3" , 2, 10},
    {"D4" , 2, 10},
    {"B5" , 2, 9},
    {"B6" , 2, 9},
    {"C11", 2, 7},
    {"C12", 2, 7},
    {"A11", 3, 0},
    {"A12", 3, 0},
    {"B7" , 5, 0},
    {"B8" , 5, 0},
    {"A9" , 5, 1},
    {"A10", 5, 1},
    {"B11", 7, 0},
    {"B12", 7, 0},
    {"A7" , 7, 1},
    {"A8" , 7, 1},
    {"B9" , 9, 0},
    {"B10", 9, 0},
    {"A5" , 9, 1},
    {"A6" , 9, 1},
    {"C7" , 9, 7},
    {"C8" , 9, 7},
    {"C3" , 9, 11},
    {"C4" , 9, 11},
    {"C5" , 7, 7},
    {"C6" , 7, 7},
    {"C15", 7, 8},
    {"C16", 7, 8},
    {"D11", 7, 10},
    {"D12", 7, 10},
    {"E11", 7, 11},
    {"E12", 7, 11},
    {"D9" , 6, 12},
    {"D10", 6, 12},
    {"D7" , 1, 12},
    {"D8" , 1, 12},
    {"E7" , 0, 11},
    {"E8" , 0, 11},
    {"C13", 0, 7},
    {"C14", 0, 7},
    {"A1" , 0, 3},
    {"A2" , 0, 3},
    {"A13", 2, 3},
    {"A14", 2, 3},
    {"A15", 4, 3},
    {"A16", 4, 3}
};

const int NUM_COORDINATES = sizeof(coordinates) / sizeof(Coordinate);

void drawTrainPosition(track_node* s1, track_node* s2) {

    char* dummy =
    "   xxxxxxxxxx\r\n"
    "    x x     x\r\n"
    "   xx xxxxxxx\r\n"
    "xx  x x  x  x\r\n"
    " x xx x  x  x\r\n"
    "xx    xxx xxx\r\n"
    " x    x     x\r\n"
    "xx    xxxxxxx\r\n"
    " x      x x  \r\n"
    "xxxxxxxxxxxxx\r\n";

    int c1 = -1;
    int c2 = -1;

    for (int i=0; i<NUM_COORDINATES; i++) {
        if (s1 && 0 == strcmp(coordinates[i].name, s1->name)) {
            c1 = coordinates[i].x*15 + coordinates[i].y;
            ASSERT(dummy[c1] == 'x', "not an x");
        }
        if (s2 && 0 == strcmp(coordinates[i].name, s2->name)) {
            c2 = coordinates[i].x*15 + coordinates[i].y;
            ASSERT(dummy[c2] == 'x', "not an x");
        }
    }

    acquireUiLock();
    uprintf(COM2, "\033[s\033[%d;1H\033[30m", TRAIN_GUI_LINENUM);
    for (char* c = dummy; *c; c++) {
        if (*c == 'x' || *c == '<' || *c == '>') {
            char charToPrint;

            if (c - dummy == c1) {
                charToPrint = '1';
            } else if (c - dummy == c2) {
                charToPrint = '2';
            } else if (*c == 'x') {
                charToPrint = ' ';
            } else {
                charToPrint = *c;
            }

            if (charToPrint == '1') {
                uprintf(COM2, "\033[%dm \033[%dm", 42, 40);
            } else if (charToPrint == '2') {
                uprintf(COM2, "\033[%dm \033[%dm", 41, 40);
            } else {
                uprintf(COM2, "\033[%dm%c\033[%dm", 47, charToPrint, 40);
            }
        } else {
            uprintf(COM2, "%c", *c);
        }
    }
    uprintf(COM2, "\033[u\033[37m");
    releaseUiLock();
}
