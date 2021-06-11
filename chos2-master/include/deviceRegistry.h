#ifndef DEVICE_REGISTRY
#define DEVICE_REGISTRY

#include <scheduler.h>

typedef struct _DeviceRegistry {
    Queue freeQueue;
    Map deviceMap;
} DeviceRegistry;

void initializeDeviceRegistry(DeviceRegistry* registry);
void WaitForInterrupt(DeviceRegistry* registry, Task* task, int device);
void WakeForInterrupt(DeviceRegistry* registry, int device);

#endif
