ADD_OBJS= stdlib.o stdio.o blkdev.o crc32.o
SOCLIB?=$(shell soclib-cc --getpath)
export SOCLIB

main.c: expected_crc.h

include $(SOCLIB)/soclib/platform/topcells/soft.mk

expected_crc.h:crc32.c mk_expected_crc.c ../test.bin
	gcc -o expected_crc crc32.c mk_expected_crc.c
	./expected_crc > $@

../test.bin:
	dd if=/dev/urandom of=$@ count=24 bs=512
