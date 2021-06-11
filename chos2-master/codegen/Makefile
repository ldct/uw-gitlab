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

LIBS = {libs}

all: kernel.elf a0.elf

kernel.s: kernel.c
	$(CC) -S $(CFLAGS) kernel.c

kernel.o: kernel.s
	$(AS) $(ASFLAGS) -o kernel.o kernel.s

kernel.elf: {kerndeps}
	$(LD) $(LDFLAGS) -o $@ kernel.o $(LIBS) $(LIBS) $(LIBS)

a0.s: a0.c
	$(CC) -S $(CFLAGS) a0.c

a0.o: a0.s
	$(AS) $(ASFLAGS) -o a0.o a0.s

a0.elf: a0.o
	$(LD) $(LDFLAGS) -o $@ a0.o -ltimer -lbwio -lgcc

{deps}

.PHONY: install clean

clean:
	-ls build/* | grep -v trackData | xargs rm -f; rm kernel.elf kernel.o kernel.s

install: kernel.elf a0.elf
	-cp kernel.elf a0.elf /u/cs452/tftp/ARM/$(shell whoami)
