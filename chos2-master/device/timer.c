#include <timer.h>
#include <ts7200.h>
#include <bwio.h>
#include <panic.h>

char* getTimerBase(int timer) {
    switch (timer){
        case 1: return (char*) TIMER1_BASE;
        case 2: return (char*) TIMER2_BASE;
        case 3: return (char*) TIMER3_BASE;
    }
    PANIC("getTimerBase: bad argument");
}

void setMode(int timer, int mode) {
    int* CRTL = (int*) (getTimerBase(timer) + CRTL_OFFSET);
    switch (mode){
    case 1:
        *CRTL |= MODE_MASK;
        break;
    default:
        *CRTL &= ~MODE_MASK;
    }
}

unsigned int sanitizeLength(int timer, unsigned int length) {
    unsigned ret = length;
    if (timer!=3){
        ret &= LOWER_MASK;
        ASSERT(ret == length, "initial timer load will overflow");
    }
    return ret;
}

void setFrequency(int timer, int frequency) {
    int* CRTL = (int*) (getTimerBase(timer) + CRTL_OFFSET);
    switch (frequency){
    case 508000:
        *CRTL |= CLKSEL_MASK;
        break;
    default:
        *CRTL &= ~CLKSEL_MASK;
    }
}

unsigned int getValue(int timer) {
    char* BASE = getTimerBase(timer);
    unsigned int value = *(int*)(BASE + VAL_OFFSET);
    if(timer!=3)
        value &= LOWER_MASK;
    return value;
}

unsigned int getWrap(int timer) {
    switch (timer){
    case 3:
        return WRAP_32;
    default:
        return WRAP_16;
    }
}

// length = initial value
// mode: 1 for pre-load (wrap-to-loaded), 0 for free-running (wrap-to-maximum)
void initializeTimer(int timer, int frequency, unsigned int length, int mode){
    char* BASE = getTimerBase(timer);
    *(int*)(BASE + CRTL_OFFSET) &= ~ENABLE_MASK;
    unsigned int sanitizedLength = sanitizeLength(timer, length);
    *(int*)(BASE + LDR_OFFSET) = sanitizedLength;
    setFrequency(timer, frequency);
    setMode(timer, mode);
    *(int*)(BASE + CRTL_OFFSET) |= ENABLE_MASK;
}

void disableTimer(int timer) {
    char* BASE = getTimerBase(timer);
    *(int*)(BASE + CRTL_OFFSET) &= ENABLE_MASK;

}
