#include <panic.h>
#include <trainnum.h>
#include <trainData.h>
#include <io.h>

const int SD_1[15] = {
    /* 0 = */   100,
    /* 1 = */   100,
    /* 2 = */   100,
    /* 3 = */   100,
    /* 4 = */   100,
    /* 5 = */   100,
    /* 6 = */   120,
    /* 7 = */   150, // real data
    /* 8 = */   220,
    /* 9 = */   330,
    /* 10 = */  440,
    /* 11 = */  620,
    /* 12 = */  800,
    /* 13 = */  1000,
    /* 14 = */  1310
};

const int SD_24[15] = {
    /* 0 = */   100,
    /* 1 = */   100,
    /* 2 = */   100,
    /* 3 = */   100,
    /* 4 = */   100,
    /* 5 = */   100,
    /* 6 = */   120,
    /* 7 = */   150,
    /* 8 = */   220,
    /* 9 = */   350, // real data
    /* 10 = */  490, 
    /* 11 = */  640,
    /* 12 = */  860,
    /* 13 = */  1120,
    /* 14 = */  1340
};

const int SD_58[15] = {
    /* 0 = */   100,
    /* 1 = */   100,
    /* 2 = */   100,
    /* 3 = */   100,
    /* 4 = */   100,
    /* 5 = */   200,
    /* 6 = */   200,
    /* 7 = */   200,
    /* 8 = */   210,
    /* 9 = */   360,
    /* 10 = */  440,
    /* 11 = */  590,
    /* 12 = */  810,
    /* 13 = */  1040,
    /* 14 = */  1340
};

const int SD_74[15] = {
    /* 0 = */   100,
    /* 1 = */   100,
    /* 2 = */   100,
    /* 3 = */   100,
    /* 4 = */   100,
    /* 5 = */   320,
    /* 6 = */   380,
    /* 7 = */   480,
    /* 8 = */   530,
    /* 9 = */   600,
    /* 10 = */  680,
    /* 11 = */  750,
    /* 12 = */  860,
    /* 13 = */  900,
    /* 14 = */  900
};

const int SD_78[15] = {
    /* 0 = */   100,
    /* 1 = */   100,
    /* 2 = */   100,
    /* 3 = */   100,
    /* 4 = */   100,
    /* 5 = */   200,
    /* 6 = */   200,
    /* 7 = */   200,
    /* 8 = */   200,
    /* 9 = */   280,
    /* 10 = */  370,
    /* 11 = */  500,
    /* 12 = */  650,
    /* 13 = */  790,
    /* 14 = */  1050
};

const int SD_79[15] = {
    /* 0 = */   1,
    /* 1 = */   1,
    /* 2 = */   1,
    /* 3 = */   20, // <- real data
    /* 4 = */   50,
    /* 5 = */   90,
    /* 6 = */   130,
    /* 7 = */   210,
    /* 8 = */   290,
    /* 9 = */   390,
    /* 10 = */  520,
    /* 11 = */  700,
    /* 12 = */  920,
    /* 13 = */  1135,
    /* 14 = */  1415
};

int getStopDistance(int trainNum, int vset) {
    ASSERT_IS_TRAINNUM(trainNum);
    ASSERT(0 <= vset, "not a vset");
    ASSERT(vset < 15, "not a vset");
    if (trainNum == 1) return SD_1[vset];
    if (trainNum == 24) return SD_24[vset];
    if (trainNum == 58) return SD_58[vset];
    if (trainNum == 74) return SD_74[vset];
    if (trainNum == 78) return SD_78[vset];
    if (trainNum == 79) return SD_79[vset];
    PANIC("???");
}
