#include <ARM.h>

void disableDCache(){
    asm("MCR p15, 0, R0, c7, c10, 4");
    int mode;
    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("MOV %0, R0":"=r"(mode));
    mode &= ~DCACHE_BIT;
    asm("MOV R0, %0"::"r"(mode));
    asm("MCR p15, 0, R0, c1, c0, 0");
}

void enableDCache(){
    asm("MCR p15, 0, R0, c7, c6, 0");
    int mode;
    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("MOV %0, R0":"=r"(mode));
    mode |= DCACHE_BIT;
    asm("MOV R0, %0"::"r"(mode));
    asm("MCR p15, 0, R0, c1, c0, 0");
}

void disableICache(){
    int mode;
    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("MOV %0, R0":"=r"(mode));
    mode &= ~ICACHE_BIT;
    asm("MOV R0, %0"::"r"(mode));
    asm("MCR p15, 0, R0, c1, c0, 0");
}

void enableICache(){
    asm("MCR p15, 0, R0, c7, c5, 0");
    int mode;
    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("MOV %0, R0":"=r"(mode));
    mode |= ICACHE_BIT;
    asm("MOV R0, %0"::"r"(mode));
    asm("MCR p15, 0, R0, c1, c0, 0");
}

void hypeTrain(){
    asm("MCR p15, 0, R0, c7, c7, 0");
    int mode;
    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("MOV %0, R0":"=r"(mode));
    mode |= ICACHE_BIT;
    mode |= DCACHE_BIT;
    asm("MOV R0, %0"::"r"(mode));
    asm("MCR p15, 0, R0, c1, c0, 0");
}

void derail(){
    asm("MCR p15, 0, R0, c7, c10, 4");
    int mode;
    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("MOV %0, R0":"=r"(mode));
    mode &= ~ICACHE_BIT;
    mode &= ~DCACHE_BIT;
    asm("MOV R0, %0"::"r"(mode));
    asm("MCR p15, 0, R0, c1, c0, 0");
}
