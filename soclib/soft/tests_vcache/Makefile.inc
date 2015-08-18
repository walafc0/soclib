all: test.elf

LDSCRIPT?=	../common/ldscript

CC=mipsel-unknown-elf-gcc
LD= mipsel-unknown-elf-ld
DU= mipsel-unknown-elf-objdump

CFLAGS=-mips32 -nostdinc -I. -I../common -mno-abicalls
LDFLAGS=-mips32 -nostdlib -Wl,-T${LDSCRIPT} -Ttext=0xbfc00000


.S.o:
	${CC} ${CFLAGS} -c -o $@ $<

# test.o has to be the first object, it's the entry point
test.elf: test.o ../common/subr.o ${EXTRA_OBJ}
	${CC} ${LDFLAGS} -o $@ $^

clean:
	rm -f test.o test.elf run.out ${EXTRA_OBJ}
