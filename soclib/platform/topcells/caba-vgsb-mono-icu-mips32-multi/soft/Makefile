LD=/users/outil/soc/cxtools/gcc_mips/obj/bin/mipsel-unknown-elf-ld
CC=/users/outil/soc/cxtools/gcc_mips/obj/bin/mipsel-unknown-elf-gcc
AS=/users/outil/soc/cxtools/gcc_mips/obj/bin/mipsel-unknown-elf-as
DU=/users/outil/soc/cxtools/gcc_mips/obj/bin/mipsel-unknown-elf-objdump

OBJS=   reset.o \
	giet.o \
        switch.o \
	syscalls.o \
	isr.o \
	stdio.o \
	main.o \

CFLAGS= -Wall -O2 -mno-gpopt -G0 -fno-builtin -fomit-frame-pointer -mips32 -ggdb

bin.soft: $(OBJS) ldscript
	$(LD) -N -q -o $@ -T ldscript $(OBJS)
	$(DU) -D $@ > $@.txt

reset.o: reset.s
	$(AS) -mips32 -o $@ $<
	$(DU) -D $@ > $@.txt

giet.o: giet.s
	$(AS) -mips32 -o $@ $<
	$(DU) -D $@ > $@.txt

switch.o: switch.s
	$(AS) -mips32 -o $@ $<
	$(DU) -D $@ > $@.txt

isr.o: isr.s
	$(AS) -mips32 -o $@ $<
	$(DU) -D $@ > $@.txt

stdio.o: stdio.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(DU) -D $@ > $@.txt

syscalls.o: syscalls.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(DU) -D $@ > $@.txt

main.o: main_tri.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(DU) -D $@ > $@.txt

clean:
	rm -f *.o bin.soft *.txt core *~
