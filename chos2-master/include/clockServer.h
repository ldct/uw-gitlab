#ifndef CLOCKSERVER_H
#define CLOCKSERVER_H

void clockServer();

void clockNotifier();

int DelayUntil(int csTid, int deadline);

int Time(int csTid);

void Delay(int csTid, int ticks);

void testNotifier();

#endif
