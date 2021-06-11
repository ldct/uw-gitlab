#ifndef TIMER_H
#define TIMER_H

/*
* some timer related stuff, hopefully useful for future endeavors.
*/

#define WRAP_16 0xffff;
#define WRAP_32 0xffffffff;
#define LOWER_MASK 0x0000ffff;

void initializeTimer(int timer, int frequency, unsigned int length, int mode);

void disableTimer();

unsigned int getValue(int timer);

unsigned int getWrap(int timer);

#endif
