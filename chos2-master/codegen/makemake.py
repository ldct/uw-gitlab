#!/usr/bin/env python3

deps = [
    'data/trackData',
    'data/trainData',
    'device/ARM',
    'device/timer',
    'experiments/measureInner',
    'experiments/measureStopDistance',
    'kern/context',
    'kern/deviceRegistry',
    'kern/initGlobals',
    'kern/interrupt',
    'kern/priorityQueue',
    'kern/scheduler',
    'kern/sendReceiveReply',
    'kern/syscall',
    'kern/task',
    'misc/bwio',
    'misc/dump',
    'misc/fast_hsv2rgb',
    'misc/panic',
    'test/k1',
    'test/k2',
    'test/k3',
    'test/k4',
    'test/maptest',
    'test/ssrTest',
    'traincontrol/doReverse',
    'traincontrol/dijkstra',
    'traincontrol/getAcceleration',
    'traincontrol/getMinLmVset',
    'traincontrol/getStopDistance',
    'traincontrol/getTravelVset',
    'traincontrol/getSpeed',
    'traincontrol/graph',
    'traincontrol/haltVisit',
    'traincontrol/instrumentedStop',
    'traincontrol/pathfinding',
    'traincontrol/reserve',
    'traincontrol/shortMove',
    'traincontrol/stopAllTrains',
    'traincontrol/tracking_globals',
    'traincontrol/wf_sensor',
    'ui/cli',
    'ui/printTrainPosition',
    'ui/programs',
    'ui/trainServerUi',
    'ui/ui',
    'user/idle',
    'user/servers/cbServer',
    'user/servers/clockServer',
    'user/servers/idlePrinter',
    'user/servers/io',
    'user/servers/nameServer',
    'user/servers/pathServer',
    'user/servers/sensorRequestor',
    'user/servers/switchServer',
    'user/servers/trainServer2',
    'user/servers/uart1',
    'user/servers/uart2',
    'user/servers/uiLockServer',
    'user/syslib',
    'user/userprogram',
    'util/charay',
    'util/estimator',
    'util/isqrt',
    'util/map',
    'util/minHeap',
    'util/parse',
    'util/queue',
    'util/trainnum'
]

def render_rule(dep):
    [folder,filename] = dep.rsplit('/', 1)
    return f"""
build/{filename}.s: {folder}/{filename}.c
	$(CC) -S $(CFLAGS) {folder}/{filename}.c -o build/{filename}.s

build/{filename}.o: build/{filename}.s
	$(AS) $(ASFLAGS) -o build/{filename}.o build/{filename}.s

build/lib{filename}.a: build/{filename}.o
	$(AR) $(ARFLAGS) $@ build/{filename}.o
"""

def render_a(dep):
    [folder,filename] = dep.rsplit('/', 1)
    return f"build/lib{filename}.a"

def render_l(dep):
    [folder,filename] = dep.rsplit('/', 1)
    return f"-l{filename}"

with open("./Makefile") as mf:
    with open("../Makefile", "w") as output:
        output.write(mf.read().format(
            libs=' '.join(render_l(dep) for dep in deps) + ' -lgcc -lstdc++ -lc',
            kerndeps="kernel.o " + ' '.join(render_a(dep) for dep in deps),
            deps=''.join(render_rule(dep) for dep in deps)
        ))
