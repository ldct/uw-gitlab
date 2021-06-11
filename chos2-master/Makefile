XDIR =/u/cs452/public/xdev/
XBINDIR=$(XDIR)bin
XLIBDIR1=$(XDIR)arm-none-eabi/lib
XLIBDIR2=$(XDIR)lib/gcc/arm-none-eabi/9.2.0
CC = $(XBINDIR)/arm-none-eabi-gcc
AS = $(XBINDIR)/arm-none-eabi-as
LD = $(XBINDIR)/arm-none-eabi-ld


ASFLAGS = -mfloat-abi=soft

# -g: include debug information for gdb
# -S: only compile and emit assembly
# -fPIC: emit position-independent code
# -Wall: report all warnings
# -mcpu=arm920t: generate code for the 920t architecture
# -msoft-float: no FP co-processor
CFLAGS = -std=gnu99 -ffreestanding -O3 -g -S -fPIC -Wall -Werror=int-conversion -Werror=pointer-arith -Werror=frame-larger-than=65536 -Werror=implicit-function-declaration -Werror=parentheses -Werror=return-type -mcpu=arm920t -msoft-float -I. -I include

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
LDFLAGS = -static -e main -nmagic -T linker.ld -L build/ -L $(XLIBDIR1) -L $(XLIBDIR2) -lc

LIBS = -ltrackData -ltrainData -lARM -ltimer -lmeasureInner -lmeasureStopDistance -lcontext -ldeviceRegistry -linitGlobals -linterrupt -lpriorityQueue -lscheduler -lsendReceiveReply -lsyscall -ltask -lbwio -ldump -lfast_hsv2rgb -lpanic -lk1 -lk2 -lk3 -lk4 -lmaptest -lssrTest -ldoReverse -ldijkstra -lgetAcceleration -lgetMinLmVset -lgetStopDistance -lgetTravelVset -lgetSpeed -lgraph -lhaltVisit -linstrumentedStop -lpathfinding -lreserve -lshortMove -lstopAllTrains -ltracking_globals -lwf_sensor -lcli -lprintTrainPosition -lprograms -ltrainServerUi -lui -lidle -lcbServer -lclockServer -lidlePrinter -lio -lnameServer -lpathServer -lsensorRequestor -lswitchServer -ltrainServer2 -luart1 -luart2 -luiLockServer -lsyslib -luserprogram -lcharay -lestimator -lisqrt -lmap -lminHeap -lparse -lqueue -ltrainnum -lgcc -lstdc++ -lc

all: kernel.elf a0.elf

kernel.s: kernel.c
	$(CC) -S $(CFLAGS) kernel.c

kernel.o: kernel.s
	$(AS) $(ASFLAGS) -o kernel.o kernel.s

kernel.elf: kernel.o build/libtrackData.a build/libtrainData.a build/libARM.a build/libtimer.a build/libmeasureInner.a build/libmeasureStopDistance.a build/libcontext.a build/libdeviceRegistry.a build/libinitGlobals.a build/libinterrupt.a build/libpriorityQueue.a build/libscheduler.a build/libsendReceiveReply.a build/libsyscall.a build/libtask.a build/libbwio.a build/libdump.a build/libfast_hsv2rgb.a build/libpanic.a build/libk1.a build/libk2.a build/libk3.a build/libk4.a build/libmaptest.a build/libssrTest.a build/libdoReverse.a build/libdijkstra.a build/libgetAcceleration.a build/libgetMinLmVset.a build/libgetStopDistance.a build/libgetTravelVset.a build/libgetSpeed.a build/libgraph.a build/libhaltVisit.a build/libinstrumentedStop.a build/libpathfinding.a build/libreserve.a build/libshortMove.a build/libstopAllTrains.a build/libtracking_globals.a build/libwf_sensor.a build/libcli.a build/libprintTrainPosition.a build/libprograms.a build/libtrainServerUi.a build/libui.a build/libidle.a build/libcbServer.a build/libclockServer.a build/libidlePrinter.a build/libio.a build/libnameServer.a build/libpathServer.a build/libsensorRequestor.a build/libswitchServer.a build/libtrainServer2.a build/libuart1.a build/libuart2.a build/libuiLockServer.a build/libsyslib.a build/libuserprogram.a build/libcharay.a build/libestimator.a build/libisqrt.a build/libmap.a build/libminHeap.a build/libparse.a build/libqueue.a build/libtrainnum.a
	$(LD) $(LDFLAGS) -o $@ kernel.o $(LIBS) $(LIBS) $(LIBS)

a0.s: a0.c
	$(CC) -S $(CFLAGS) a0.c

a0.o: a0.s
	$(AS) $(ASFLAGS) -o a0.o a0.s

a0.elf: a0.o
	$(LD) $(LDFLAGS) -o $@ a0.o -ltimer -lbwio -lgcc


build/trackData.s: data/trackData.c
	$(CC) -S $(CFLAGS) data/trackData.c -o build/trackData.s

build/trackData.o: build/trackData.s
	$(AS) $(ASFLAGS) -o build/trackData.o build/trackData.s

build/libtrackData.a: build/trackData.o
	$(AR) $(ARFLAGS) $@ build/trackData.o

build/trainData.s: data/trainData.c
	$(CC) -S $(CFLAGS) data/trainData.c -o build/trainData.s

build/trainData.o: build/trainData.s
	$(AS) $(ASFLAGS) -o build/trainData.o build/trainData.s

build/libtrainData.a: build/trainData.o
	$(AR) $(ARFLAGS) $@ build/trainData.o

build/ARM.s: device/ARM.c
	$(CC) -S $(CFLAGS) device/ARM.c -o build/ARM.s

build/ARM.o: build/ARM.s
	$(AS) $(ASFLAGS) -o build/ARM.o build/ARM.s

build/libARM.a: build/ARM.o
	$(AR) $(ARFLAGS) $@ build/ARM.o

build/timer.s: device/timer.c
	$(CC) -S $(CFLAGS) device/timer.c -o build/timer.s

build/timer.o: build/timer.s
	$(AS) $(ASFLAGS) -o build/timer.o build/timer.s

build/libtimer.a: build/timer.o
	$(AR) $(ARFLAGS) $@ build/timer.o

build/measureInner.s: experiments/measureInner.c
	$(CC) -S $(CFLAGS) experiments/measureInner.c -o build/measureInner.s

build/measureInner.o: build/measureInner.s
	$(AS) $(ASFLAGS) -o build/measureInner.o build/measureInner.s

build/libmeasureInner.a: build/measureInner.o
	$(AR) $(ARFLAGS) $@ build/measureInner.o

build/measureStopDistance.s: experiments/measureStopDistance.c
	$(CC) -S $(CFLAGS) experiments/measureStopDistance.c -o build/measureStopDistance.s

build/measureStopDistance.o: build/measureStopDistance.s
	$(AS) $(ASFLAGS) -o build/measureStopDistance.o build/measureStopDistance.s

build/libmeasureStopDistance.a: build/measureStopDistance.o
	$(AR) $(ARFLAGS) $@ build/measureStopDistance.o

build/context.s: kern/context.c
	$(CC) -S $(CFLAGS) kern/context.c -o build/context.s

build/context.o: build/context.s
	$(AS) $(ASFLAGS) -o build/context.o build/context.s

build/libcontext.a: build/context.o
	$(AR) $(ARFLAGS) $@ build/context.o

build/deviceRegistry.s: kern/deviceRegistry.c
	$(CC) -S $(CFLAGS) kern/deviceRegistry.c -o build/deviceRegistry.s

build/deviceRegistry.o: build/deviceRegistry.s
	$(AS) $(ASFLAGS) -o build/deviceRegistry.o build/deviceRegistry.s

build/libdeviceRegistry.a: build/deviceRegistry.o
	$(AR) $(ARFLAGS) $@ build/deviceRegistry.o

build/initGlobals.s: kern/initGlobals.c
	$(CC) -S $(CFLAGS) kern/initGlobals.c -o build/initGlobals.s

build/initGlobals.o: build/initGlobals.s
	$(AS) $(ASFLAGS) -o build/initGlobals.o build/initGlobals.s

build/libinitGlobals.a: build/initGlobals.o
	$(AR) $(ARFLAGS) $@ build/initGlobals.o

build/interrupt.s: kern/interrupt.c
	$(CC) -S $(CFLAGS) kern/interrupt.c -o build/interrupt.s

build/interrupt.o: build/interrupt.s
	$(AS) $(ASFLAGS) -o build/interrupt.o build/interrupt.s

build/libinterrupt.a: build/interrupt.o
	$(AR) $(ARFLAGS) $@ build/interrupt.o

build/priorityQueue.s: kern/priorityQueue.c
	$(CC) -S $(CFLAGS) kern/priorityQueue.c -o build/priorityQueue.s

build/priorityQueue.o: build/priorityQueue.s
	$(AS) $(ASFLAGS) -o build/priorityQueue.o build/priorityQueue.s

build/libpriorityQueue.a: build/priorityQueue.o
	$(AR) $(ARFLAGS) $@ build/priorityQueue.o

build/scheduler.s: kern/scheduler.c
	$(CC) -S $(CFLAGS) kern/scheduler.c -o build/scheduler.s

build/scheduler.o: build/scheduler.s
	$(AS) $(ASFLAGS) -o build/scheduler.o build/scheduler.s

build/libscheduler.a: build/scheduler.o
	$(AR) $(ARFLAGS) $@ build/scheduler.o

build/sendReceiveReply.s: kern/sendReceiveReply.c
	$(CC) -S $(CFLAGS) kern/sendReceiveReply.c -o build/sendReceiveReply.s

build/sendReceiveReply.o: build/sendReceiveReply.s
	$(AS) $(ASFLAGS) -o build/sendReceiveReply.o build/sendReceiveReply.s

build/libsendReceiveReply.a: build/sendReceiveReply.o
	$(AR) $(ARFLAGS) $@ build/sendReceiveReply.o

build/syscall.s: kern/syscall.c
	$(CC) -S $(CFLAGS) kern/syscall.c -o build/syscall.s

build/syscall.o: build/syscall.s
	$(AS) $(ASFLAGS) -o build/syscall.o build/syscall.s

build/libsyscall.a: build/syscall.o
	$(AR) $(ARFLAGS) $@ build/syscall.o

build/task.s: kern/task.c
	$(CC) -S $(CFLAGS) kern/task.c -o build/task.s

build/task.o: build/task.s
	$(AS) $(ASFLAGS) -o build/task.o build/task.s

build/libtask.a: build/task.o
	$(AR) $(ARFLAGS) $@ build/task.o

build/bwio.s: misc/bwio.c
	$(CC) -S $(CFLAGS) misc/bwio.c -o build/bwio.s

build/bwio.o: build/bwio.s
	$(AS) $(ASFLAGS) -o build/bwio.o build/bwio.s

build/libbwio.a: build/bwio.o
	$(AR) $(ARFLAGS) $@ build/bwio.o

build/dump.s: misc/dump.c
	$(CC) -S $(CFLAGS) misc/dump.c -o build/dump.s

build/dump.o: build/dump.s
	$(AS) $(ASFLAGS) -o build/dump.o build/dump.s

build/libdump.a: build/dump.o
	$(AR) $(ARFLAGS) $@ build/dump.o

build/fast_hsv2rgb.s: misc/fast_hsv2rgb.c
	$(CC) -S $(CFLAGS) misc/fast_hsv2rgb.c -o build/fast_hsv2rgb.s

build/fast_hsv2rgb.o: build/fast_hsv2rgb.s
	$(AS) $(ASFLAGS) -o build/fast_hsv2rgb.o build/fast_hsv2rgb.s

build/libfast_hsv2rgb.a: build/fast_hsv2rgb.o
	$(AR) $(ARFLAGS) $@ build/fast_hsv2rgb.o

build/panic.s: misc/panic.c
	$(CC) -S $(CFLAGS) misc/panic.c -o build/panic.s

build/panic.o: build/panic.s
	$(AS) $(ASFLAGS) -o build/panic.o build/panic.s

build/libpanic.a: build/panic.o
	$(AR) $(ARFLAGS) $@ build/panic.o

build/k1.s: test/k1.c
	$(CC) -S $(CFLAGS) test/k1.c -o build/k1.s

build/k1.o: build/k1.s
	$(AS) $(ASFLAGS) -o build/k1.o build/k1.s

build/libk1.a: build/k1.o
	$(AR) $(ARFLAGS) $@ build/k1.o

build/k2.s: test/k2.c
	$(CC) -S $(CFLAGS) test/k2.c -o build/k2.s

build/k2.o: build/k2.s
	$(AS) $(ASFLAGS) -o build/k2.o build/k2.s

build/libk2.a: build/k2.o
	$(AR) $(ARFLAGS) $@ build/k2.o

build/k3.s: test/k3.c
	$(CC) -S $(CFLAGS) test/k3.c -o build/k3.s

build/k3.o: build/k3.s
	$(AS) $(ASFLAGS) -o build/k3.o build/k3.s

build/libk3.a: build/k3.o
	$(AR) $(ARFLAGS) $@ build/k3.o

build/k4.s: test/k4.c
	$(CC) -S $(CFLAGS) test/k4.c -o build/k4.s

build/k4.o: build/k4.s
	$(AS) $(ASFLAGS) -o build/k4.o build/k4.s

build/libk4.a: build/k4.o
	$(AR) $(ARFLAGS) $@ build/k4.o

build/maptest.s: test/maptest.c
	$(CC) -S $(CFLAGS) test/maptest.c -o build/maptest.s

build/maptest.o: build/maptest.s
	$(AS) $(ASFLAGS) -o build/maptest.o build/maptest.s

build/libmaptest.a: build/maptest.o
	$(AR) $(ARFLAGS) $@ build/maptest.o

build/ssrTest.s: test/ssrTest.c
	$(CC) -S $(CFLAGS) test/ssrTest.c -o build/ssrTest.s

build/ssrTest.o: build/ssrTest.s
	$(AS) $(ASFLAGS) -o build/ssrTest.o build/ssrTest.s

build/libssrTest.a: build/ssrTest.o
	$(AR) $(ARFLAGS) $@ build/ssrTest.o

build/doReverse.s: traincontrol/doReverse.c
	$(CC) -S $(CFLAGS) traincontrol/doReverse.c -o build/doReverse.s

build/doReverse.o: build/doReverse.s
	$(AS) $(ASFLAGS) -o build/doReverse.o build/doReverse.s

build/libdoReverse.a: build/doReverse.o
	$(AR) $(ARFLAGS) $@ build/doReverse.o

build/dijkstra.s: traincontrol/dijkstra.c
	$(CC) -S $(CFLAGS) traincontrol/dijkstra.c -o build/dijkstra.s

build/dijkstra.o: build/dijkstra.s
	$(AS) $(ASFLAGS) -o build/dijkstra.o build/dijkstra.s

build/libdijkstra.a: build/dijkstra.o
	$(AR) $(ARFLAGS) $@ build/dijkstra.o

build/getAcceleration.s: traincontrol/getAcceleration.c
	$(CC) -S $(CFLAGS) traincontrol/getAcceleration.c -o build/getAcceleration.s

build/getAcceleration.o: build/getAcceleration.s
	$(AS) $(ASFLAGS) -o build/getAcceleration.o build/getAcceleration.s

build/libgetAcceleration.a: build/getAcceleration.o
	$(AR) $(ARFLAGS) $@ build/getAcceleration.o

build/getMinLmVset.s: traincontrol/getMinLmVset.c
	$(CC) -S $(CFLAGS) traincontrol/getMinLmVset.c -o build/getMinLmVset.s

build/getMinLmVset.o: build/getMinLmVset.s
	$(AS) $(ASFLAGS) -o build/getMinLmVset.o build/getMinLmVset.s

build/libgetMinLmVset.a: build/getMinLmVset.o
	$(AR) $(ARFLAGS) $@ build/getMinLmVset.o

build/getStopDistance.s: traincontrol/getStopDistance.c
	$(CC) -S $(CFLAGS) traincontrol/getStopDistance.c -o build/getStopDistance.s

build/getStopDistance.o: build/getStopDistance.s
	$(AS) $(ASFLAGS) -o build/getStopDistance.o build/getStopDistance.s

build/libgetStopDistance.a: build/getStopDistance.o
	$(AR) $(ARFLAGS) $@ build/getStopDistance.o

build/getTravelVset.s: traincontrol/getTravelVset.c
	$(CC) -S $(CFLAGS) traincontrol/getTravelVset.c -o build/getTravelVset.s

build/getTravelVset.o: build/getTravelVset.s
	$(AS) $(ASFLAGS) -o build/getTravelVset.o build/getTravelVset.s

build/libgetTravelVset.a: build/getTravelVset.o
	$(AR) $(ARFLAGS) $@ build/getTravelVset.o

build/getSpeed.s: traincontrol/getSpeed.c
	$(CC) -S $(CFLAGS) traincontrol/getSpeed.c -o build/getSpeed.s

build/getSpeed.o: build/getSpeed.s
	$(AS) $(ASFLAGS) -o build/getSpeed.o build/getSpeed.s

build/libgetSpeed.a: build/getSpeed.o
	$(AR) $(ARFLAGS) $@ build/getSpeed.o

build/graph.s: traincontrol/graph.c
	$(CC) -S $(CFLAGS) traincontrol/graph.c -o build/graph.s

build/graph.o: build/graph.s
	$(AS) $(ASFLAGS) -o build/graph.o build/graph.s

build/libgraph.a: build/graph.o
	$(AR) $(ARFLAGS) $@ build/graph.o

build/haltVisit.s: traincontrol/haltVisit.c
	$(CC) -S $(CFLAGS) traincontrol/haltVisit.c -o build/haltVisit.s

build/haltVisit.o: build/haltVisit.s
	$(AS) $(ASFLAGS) -o build/haltVisit.o build/haltVisit.s

build/libhaltVisit.a: build/haltVisit.o
	$(AR) $(ARFLAGS) $@ build/haltVisit.o

build/instrumentedStop.s: traincontrol/instrumentedStop.c
	$(CC) -S $(CFLAGS) traincontrol/instrumentedStop.c -o build/instrumentedStop.s

build/instrumentedStop.o: build/instrumentedStop.s
	$(AS) $(ASFLAGS) -o build/instrumentedStop.o build/instrumentedStop.s

build/libinstrumentedStop.a: build/instrumentedStop.o
	$(AR) $(ARFLAGS) $@ build/instrumentedStop.o

build/pathfinding.s: traincontrol/pathfinding.c
	$(CC) -S $(CFLAGS) traincontrol/pathfinding.c -o build/pathfinding.s

build/pathfinding.o: build/pathfinding.s
	$(AS) $(ASFLAGS) -o build/pathfinding.o build/pathfinding.s

build/libpathfinding.a: build/pathfinding.o
	$(AR) $(ARFLAGS) $@ build/pathfinding.o

build/reserve.s: traincontrol/reserve.c
	$(CC) -S $(CFLAGS) traincontrol/reserve.c -o build/reserve.s

build/reserve.o: build/reserve.s
	$(AS) $(ASFLAGS) -o build/reserve.o build/reserve.s

build/libreserve.a: build/reserve.o
	$(AR) $(ARFLAGS) $@ build/reserve.o

build/shortMove.s: traincontrol/shortMove.c
	$(CC) -S $(CFLAGS) traincontrol/shortMove.c -o build/shortMove.s

build/shortMove.o: build/shortMove.s
	$(AS) $(ASFLAGS) -o build/shortMove.o build/shortMove.s

build/libshortMove.a: build/shortMove.o
	$(AR) $(ARFLAGS) $@ build/shortMove.o

build/stopAllTrains.s: traincontrol/stopAllTrains.c
	$(CC) -S $(CFLAGS) traincontrol/stopAllTrains.c -o build/stopAllTrains.s

build/stopAllTrains.o: build/stopAllTrains.s
	$(AS) $(ASFLAGS) -o build/stopAllTrains.o build/stopAllTrains.s

build/libstopAllTrains.a: build/stopAllTrains.o
	$(AR) $(ARFLAGS) $@ build/stopAllTrains.o

build/tracking_globals.s: traincontrol/tracking_globals.c
	$(CC) -S $(CFLAGS) traincontrol/tracking_globals.c -o build/tracking_globals.s

build/tracking_globals.o: build/tracking_globals.s
	$(AS) $(ASFLAGS) -o build/tracking_globals.o build/tracking_globals.s

build/libtracking_globals.a: build/tracking_globals.o
	$(AR) $(ARFLAGS) $@ build/tracking_globals.o

build/wf_sensor.s: traincontrol/wf_sensor.c
	$(CC) -S $(CFLAGS) traincontrol/wf_sensor.c -o build/wf_sensor.s

build/wf_sensor.o: build/wf_sensor.s
	$(AS) $(ASFLAGS) -o build/wf_sensor.o build/wf_sensor.s

build/libwf_sensor.a: build/wf_sensor.o
	$(AR) $(ARFLAGS) $@ build/wf_sensor.o

build/cli.s: ui/cli.c
	$(CC) -S $(CFLAGS) ui/cli.c -o build/cli.s

build/cli.o: build/cli.s
	$(AS) $(ASFLAGS) -o build/cli.o build/cli.s

build/libcli.a: build/cli.o
	$(AR) $(ARFLAGS) $@ build/cli.o

build/printTrainPosition.s: ui/printTrainPosition.c
	$(CC) -S $(CFLAGS) ui/printTrainPosition.c -o build/printTrainPosition.s

build/printTrainPosition.o: build/printTrainPosition.s
	$(AS) $(ASFLAGS) -o build/printTrainPosition.o build/printTrainPosition.s

build/libprintTrainPosition.a: build/printTrainPosition.o
	$(AR) $(ARFLAGS) $@ build/printTrainPosition.o

build/programs.s: ui/programs.c
	$(CC) -S $(CFLAGS) ui/programs.c -o build/programs.s

build/programs.o: build/programs.s
	$(AS) $(ASFLAGS) -o build/programs.o build/programs.s

build/libprograms.a: build/programs.o
	$(AR) $(ARFLAGS) $@ build/programs.o

build/trainServerUi.s: ui/trainServerUi.c
	$(CC) -S $(CFLAGS) ui/trainServerUi.c -o build/trainServerUi.s

build/trainServerUi.o: build/trainServerUi.s
	$(AS) $(ASFLAGS) -o build/trainServerUi.o build/trainServerUi.s

build/libtrainServerUi.a: build/trainServerUi.o
	$(AR) $(ARFLAGS) $@ build/trainServerUi.o

build/ui.s: ui/ui.c
	$(CC) -S $(CFLAGS) ui/ui.c -o build/ui.s

build/ui.o: build/ui.s
	$(AS) $(ASFLAGS) -o build/ui.o build/ui.s

build/libui.a: build/ui.o
	$(AR) $(ARFLAGS) $@ build/ui.o

build/idle.s: user/idle.c
	$(CC) -S $(CFLAGS) user/idle.c -o build/idle.s

build/idle.o: build/idle.s
	$(AS) $(ASFLAGS) -o build/idle.o build/idle.s

build/libidle.a: build/idle.o
	$(AR) $(ARFLAGS) $@ build/idle.o

build/cbServer.s: user/servers/cbServer.c
	$(CC) -S $(CFLAGS) user/servers/cbServer.c -o build/cbServer.s

build/cbServer.o: build/cbServer.s
	$(AS) $(ASFLAGS) -o build/cbServer.o build/cbServer.s

build/libcbServer.a: build/cbServer.o
	$(AR) $(ARFLAGS) $@ build/cbServer.o

build/clockServer.s: user/servers/clockServer.c
	$(CC) -S $(CFLAGS) user/servers/clockServer.c -o build/clockServer.s

build/clockServer.o: build/clockServer.s
	$(AS) $(ASFLAGS) -o build/clockServer.o build/clockServer.s

build/libclockServer.a: build/clockServer.o
	$(AR) $(ARFLAGS) $@ build/clockServer.o

build/idlePrinter.s: user/servers/idlePrinter.c
	$(CC) -S $(CFLAGS) user/servers/idlePrinter.c -o build/idlePrinter.s

build/idlePrinter.o: build/idlePrinter.s
	$(AS) $(ASFLAGS) -o build/idlePrinter.o build/idlePrinter.s

build/libidlePrinter.a: build/idlePrinter.o
	$(AR) $(ARFLAGS) $@ build/idlePrinter.o

build/io.s: user/servers/io.c
	$(CC) -S $(CFLAGS) user/servers/io.c -o build/io.s

build/io.o: build/io.s
	$(AS) $(ASFLAGS) -o build/io.o build/io.s

build/libio.a: build/io.o
	$(AR) $(ARFLAGS) $@ build/io.o

build/nameServer.s: user/servers/nameServer.c
	$(CC) -S $(CFLAGS) user/servers/nameServer.c -o build/nameServer.s

build/nameServer.o: build/nameServer.s
	$(AS) $(ASFLAGS) -o build/nameServer.o build/nameServer.s

build/libnameServer.a: build/nameServer.o
	$(AR) $(ARFLAGS) $@ build/nameServer.o

build/pathServer.s: user/servers/pathServer.c
	$(CC) -S $(CFLAGS) user/servers/pathServer.c -o build/pathServer.s

build/pathServer.o: build/pathServer.s
	$(AS) $(ASFLAGS) -o build/pathServer.o build/pathServer.s

build/libpathServer.a: build/pathServer.o
	$(AR) $(ARFLAGS) $@ build/pathServer.o

build/sensorRequestor.s: user/servers/sensorRequestor.c
	$(CC) -S $(CFLAGS) user/servers/sensorRequestor.c -o build/sensorRequestor.s

build/sensorRequestor.o: build/sensorRequestor.s
	$(AS) $(ASFLAGS) -o build/sensorRequestor.o build/sensorRequestor.s

build/libsensorRequestor.a: build/sensorRequestor.o
	$(AR) $(ARFLAGS) $@ build/sensorRequestor.o

build/switchServer.s: user/servers/switchServer.c
	$(CC) -S $(CFLAGS) user/servers/switchServer.c -o build/switchServer.s

build/switchServer.o: build/switchServer.s
	$(AS) $(ASFLAGS) -o build/switchServer.o build/switchServer.s

build/libswitchServer.a: build/switchServer.o
	$(AR) $(ARFLAGS) $@ build/switchServer.o

build/trainServer2.s: user/servers/trainServer2.c
	$(CC) -S $(CFLAGS) user/servers/trainServer2.c -o build/trainServer2.s

build/trainServer2.o: build/trainServer2.s
	$(AS) $(ASFLAGS) -o build/trainServer2.o build/trainServer2.s

build/libtrainServer2.a: build/trainServer2.o
	$(AR) $(ARFLAGS) $@ build/trainServer2.o

build/uart1.s: user/servers/uart1.c
	$(CC) -S $(CFLAGS) user/servers/uart1.c -o build/uart1.s

build/uart1.o: build/uart1.s
	$(AS) $(ASFLAGS) -o build/uart1.o build/uart1.s

build/libuart1.a: build/uart1.o
	$(AR) $(ARFLAGS) $@ build/uart1.o

build/uart2.s: user/servers/uart2.c
	$(CC) -S $(CFLAGS) user/servers/uart2.c -o build/uart2.s

build/uart2.o: build/uart2.s
	$(AS) $(ASFLAGS) -o build/uart2.o build/uart2.s

build/libuart2.a: build/uart2.o
	$(AR) $(ARFLAGS) $@ build/uart2.o

build/uiLockServer.s: user/servers/uiLockServer.c
	$(CC) -S $(CFLAGS) user/servers/uiLockServer.c -o build/uiLockServer.s

build/uiLockServer.o: build/uiLockServer.s
	$(AS) $(ASFLAGS) -o build/uiLockServer.o build/uiLockServer.s

build/libuiLockServer.a: build/uiLockServer.o
	$(AR) $(ARFLAGS) $@ build/uiLockServer.o

build/syslib.s: user/syslib.c
	$(CC) -S $(CFLAGS) user/syslib.c -o build/syslib.s

build/syslib.o: build/syslib.s
	$(AS) $(ASFLAGS) -o build/syslib.o build/syslib.s

build/libsyslib.a: build/syslib.o
	$(AR) $(ARFLAGS) $@ build/syslib.o

build/userprogram.s: user/userprogram.c
	$(CC) -S $(CFLAGS) user/userprogram.c -o build/userprogram.s

build/userprogram.o: build/userprogram.s
	$(AS) $(ASFLAGS) -o build/userprogram.o build/userprogram.s

build/libuserprogram.a: build/userprogram.o
	$(AR) $(ARFLAGS) $@ build/userprogram.o

build/charay.s: util/charay.c
	$(CC) -S $(CFLAGS) util/charay.c -o build/charay.s

build/charay.o: build/charay.s
	$(AS) $(ASFLAGS) -o build/charay.o build/charay.s

build/libcharay.a: build/charay.o
	$(AR) $(ARFLAGS) $@ build/charay.o

build/estimator.s: util/estimator.c
	$(CC) -S $(CFLAGS) util/estimator.c -o build/estimator.s

build/estimator.o: build/estimator.s
	$(AS) $(ASFLAGS) -o build/estimator.o build/estimator.s

build/libestimator.a: build/estimator.o
	$(AR) $(ARFLAGS) $@ build/estimator.o

build/isqrt.s: util/isqrt.c
	$(CC) -S $(CFLAGS) util/isqrt.c -o build/isqrt.s

build/isqrt.o: build/isqrt.s
	$(AS) $(ASFLAGS) -o build/isqrt.o build/isqrt.s

build/libisqrt.a: build/isqrt.o
	$(AR) $(ARFLAGS) $@ build/isqrt.o

build/map.s: util/map.c
	$(CC) -S $(CFLAGS) util/map.c -o build/map.s

build/map.o: build/map.s
	$(AS) $(ASFLAGS) -o build/map.o build/map.s

build/libmap.a: build/map.o
	$(AR) $(ARFLAGS) $@ build/map.o

build/minHeap.s: util/minHeap.c
	$(CC) -S $(CFLAGS) util/minHeap.c -o build/minHeap.s

build/minHeap.o: build/minHeap.s
	$(AS) $(ASFLAGS) -o build/minHeap.o build/minHeap.s

build/libminHeap.a: build/minHeap.o
	$(AR) $(ARFLAGS) $@ build/minHeap.o

build/parse.s: util/parse.c
	$(CC) -S $(CFLAGS) util/parse.c -o build/parse.s

build/parse.o: build/parse.s
	$(AS) $(ASFLAGS) -o build/parse.o build/parse.s

build/libparse.a: build/parse.o
	$(AR) $(ARFLAGS) $@ build/parse.o

build/queue.s: util/queue.c
	$(CC) -S $(CFLAGS) util/queue.c -o build/queue.s

build/queue.o: build/queue.s
	$(AS) $(ASFLAGS) -o build/queue.o build/queue.s

build/libqueue.a: build/queue.o
	$(AR) $(ARFLAGS) $@ build/queue.o

build/trainnum.s: util/trainnum.c
	$(CC) -S $(CFLAGS) util/trainnum.c -o build/trainnum.s

build/trainnum.o: build/trainnum.s
	$(AS) $(ASFLAGS) -o build/trainnum.o build/trainnum.s

build/libtrainnum.a: build/trainnum.o
	$(AR) $(ARFLAGS) $@ build/trainnum.o


.PHONY: install clean

clean:
	-ls build/* | grep -v trackData | xargs rm -f; rm kernel.elf kernel.o kernel.s

install: kernel.elf a0.elf
	-cp kernel.elf a0.elf /u/cs452/tftp/ARM/$(shell whoami)
