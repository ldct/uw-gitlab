#ifndef SWITCHSERVER_H
#define SWITCHSERVER_H

#define NUM_SWITCHES 22

// todo: provide this via api
extern char switch_state[NUM_SWITCHES];

int switchIndex(int address);

void switchStraight();

void switchCurved();

void setInitialSwitches();

void setTurnout(int number, int position);

void setTurnoutNB(int number, int position);

void initSwitchServer();

void printSwitchStatus();

#endif
