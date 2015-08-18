CC=mipsel-unknown-elf-gcc
AS=mipsel-unknown-elf-as
LD=mipsel-unknown-elf-ld
DU=mipsel-unknown-elf-objdump

SYS_OBJS = reset.o \
	   giet.o \
	   common.o \
	   ctx_handler.o \
	   drivers.o \
	   exc_handler.o \
	   irq_handler.o \
	   sys_handler.o

APP_OBJS = stdio.o \
	   main.o

SYS_PATH=sys
APP_PATH=app

SYS_CFLAGS=-Wall -ffreestanding -mno-gpopt -mips32 -I$(SYS_PATH) -I.
APP_CFLAGS=-Wall -ffreestanding -mno-gpopt -mips32 -I$(APP_PATH) -I.

all: sys.bin app.bin

## system compilation

sys.bin: $(SYS_OBJS) sys.ld seg.ld
	$(LD) -o $@ -T sys.ld $(SYS_OBJS)
	$(DU) -D $@ > $@.txt

reset.o: $(SYS_PATH)/reset.s
	$(AS) -g -mips32 -o $@ $<

giet.o: $(SYS_PATH)/giet.s
	$(AS) -g -mips32 -o $@ $<

%.o: $(SYS_PATH)/%.c $(SYS_PATH)/%.h
	$(CC) $(SYS_CFLAGS) -c -o $@ $<

## app compilation

app.bin: $(APP_OBJS) app.ld seg.ld
	$(LD) -o $@ -T app.ld $(APP_OBJS)
	$(DU) -D $@ > $@.txt

stdio.o: $(APP_PATH)/stdio.c $(APP_PATH)/stdio.h
	$(CC) $(APP_CFLAGS) -c -o $@ $<

main.o: $(APP_PATH)/main.c
	$(CC) $(APP_CFLAGS) -c -o $@ $<

### special rules

clean:
	rm -f *.o *.bin *.txt core *~ proc0*

SIMUL_PATH?=../../../hard/almo_generic
run: all
	$(SIMUL_PATH)/simul_almo_generic -SYS sys.bin -APP app.bin
