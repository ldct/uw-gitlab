#include <panic.h>
#include <trainnum.h>
#include <io.h>

// speed from above
int SPEED_1[15] = {
    /* 0 = */   1,
    /* 1 = */   1,
    /* 2 = */   1,
    /* 3 = */   1,
    /* 4 = */   1,
    /* 5 = */   1,
    /* 6 = */   132,
    /* 7 = */   184,
    /* 8 = */   240,
    /* 9 = */   300,
    /* 10 = */  360,
    /* 11 = */  429,
    /* 12 = */  486,
    /* 13 = */  524,
    /* 14 = */  586
};

/* train 1 speed from below
    11 = 390,
    12 = 460,
    13 = 520,
    14 = 594
*/


// speed from above
int SPEED_24[15] = {
    /* 0 = */   1,
    /* 1 = */   1,
    /* 2 = */   1,
    /* 3 = */   1,
    /* 4 = */   1,
    /* 5 = */   1,
    /* 6 = */   137,
    /* 7 = */   190,
    /* 8 = */   247,
    /* 9 = */   313,
    /* 10 = */  369,
    /* 11 = */  449, // 410
    /* 12 = */  512, // 479
    /* 13 = */  584, // 548
    /* 14 = */  626  // 620
};

int SPEED_74[15] = {
    /* 0 = */   1,
    /* 1 = */   1,
    /* 2 = */   1,
    /* 3 = */   1,
    /* 4 = */   170, // real data
    /* 5 = */   218,
    /* 6 = */   269,
    /* 7 = */   328,
    /* 8 = */   383,
    /* 9 = */   435,
    /* 10 = */  483,
    /* 11 = */  538,
    /* 12 = */  581,
    /* 13 = */  599,
    /* 14 = */  599
};

int SPEED_58[15] = {
    /* 0 = */   1,
    /* 1 = */   1,
    /* 2 = */   1,
    /* 3 = */   1,
    /* 4 = */   1,
    /* 5 = */   40,
    /* 6 = */   103,
    /* 7 = */   169, // real data
    /* 8 = */   221,
    /* 9 = */   277,
    /* 10 = */  334,
    /* 11 = */  407,
    /* 12 = */  473,
    /* 13 = */  513,
    /* 14 = */  585
};

int SPEED_78[15] = {
    /* 0 = */   1,
    /* 1 = */   1,
    /* 2 = */   1,
    /* 3 = */   1,
    /* 4 = */   1,
    /* 5 = */   34,
    /* 6 = */   86,
    /* 7 = */   138,
    /* 8 = */   196, // real data
    /* 9 = */   245,
    /* 10 = */  268,
    /* 11 = */  357,
    /* 12 = */  409,
    /* 13 = */  460,
    /* 14 = */  491
};

int SPEED_79[15] = {
    /* 0 = */   1,
    /* 1 = */   17,
    /* 2 = */   36,
    /* 3 = */   60,
    /* 4 = */   89,
    /* 5 = */   134,
    /* 6 = */   170,
    /* 7 = */   224,
    /* 8 = */   276,
    /* 9 = */   340,
    /* 10 = */  407,
    /* 11 = */  477,
    /* 12 = */  545,
    /* 13 = */  623,
    /* 14 = */  664
};

int getSpeed(int trainNum, int vset) {
    ASSERT_IS_TRAINNUM(trainNum);
    ASSERT(0 <= vset, "not a vset");
    ASSERT(vset < 15, "not a vset");
    if (trainNum == 1) return SPEED_1[vset];
    if (trainNum == 24) return SPEED_24[vset];
    if (trainNum == 58) return SPEED_58[vset];
    if (trainNum == 74) return SPEED_74[vset];
    if (trainNum == 78) return SPEED_78[vset];
    if (trainNum == 79) return SPEED_79[vset];

    PANIC("???");
}

void updateSpeedEstimate(int trainNum, int vset, int newSpeed) {
    ASSERT_IS_TRAINNUM(trainNum);
    ASSERT(0 <= vset, "not a vset");
    ASSERT(vset < 15, "not a vset");
    if (trainNum == 58) SPEED_58[vset] = 0.9 * SPEED_58[vset] + 0.1 + newSpeed;
    if (trainNum == 74) SPEED_74[vset] = 0.9 * SPEED_74[vset] + 0.1 + newSpeed;
    if (trainNum == 78) SPEED_78[vset] = 0.9 * SPEED_78[vset] + 0.1 + newSpeed;
}
