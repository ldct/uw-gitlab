#include <io.h>
#include <tracknode.h>
#include <wf_sensor.h>
#include <pathfinding.h>
#include <instrumentedStop.h>

void measureStopDistance(const char* sensorName) {
    track_node* sensor = trackNodeFromName(sensorName);
    wf_Sensor(sensor);
    instrumentedStop("sd", 79);
}
