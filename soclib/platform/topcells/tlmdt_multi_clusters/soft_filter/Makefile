LD=mipsel-unknown-elf-ld
CC=mipsel-unknown-elf-gcc
AS=mipsel-unknown-elf-as
DU=mipsel-unknown-elf-objdump

OBJS=   reset.o \
	giet.o \
	isr.o \
	drivers.o \
	stdio.o \
	main.o

CFLAGS= -Wall -mno-gpopt -ffreestanding -fomit-frame-pointer -mips32 -ggdb

#GIET=	/Users/alain/Documents/licence/almo/soft/giet

bin.soft: $(OBJS) ldscript
	$(LD) -o $@ -T ldscript $(OBJS)
	$(DU) -D $@ > $@.txt

reset.o: reset.s
	$(AS) -g -mips32 -o $@ $<
	$(DU) -D $@ > $@.txt

giet.o: $(GIET)/giet.s
	$(AS) -g -mips32 -o $@ $<
	$(DU) -D $@ > $@.txt

isr.o: $(GIET)/isr.s
	$(AS) -g -mips32 -o $@ $<
	$(DU) -D $@ > $@.txt

stdio.o: $(GIET)/stdio.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(DU) -D $@ > $@.txt

drivers.o: $(GIET)/drivers.c
	$(CC) $(CFLAGS) -c -o $@ $<
	$(DU) -D $@ > $@.txt

main.o: main.c
	$(CC) $(CFLAGS) -I$(GIET) -c -o $@ $<
	$(DU) -D $@ > $@.txt

clean:
	rm -f *.o bin.soft *.txt core *~ proc* term* temp
