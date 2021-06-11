#ifndef TRAINSERVER2_H
#define TRAINSERVER2_H

#include <tracknode.h>

void trainServer2();

track_node* whereIsTrain(int trainNum);
int whatVset(int trainNum);
void tsSetVset(int trainNum, int speed);

#endif
