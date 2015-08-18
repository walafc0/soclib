CFLAGS=-Wall -mno-xl-soft-div -mno-xl-soft-mul -mxl-barrel-shift -O3
bin.soft: a.out
a.out: boot.o it.o main.o lee.o
	microblaze-ld -o a.out -T ./ldscript boot.o it.o main.o lee.o
main.o: main.c
	microblaze-gcc $(CFLAGS) -c $<
lee.o: lee.c
	microblaze-gcc $(CFLAGS) -c $<
boot.o: boot.s
	microblaze-gcc $(CFLAGS) -c $<
it.o: it.s
	microblaze-gcc $(CFLAGS) -c $<
clean:
	rm -f it.o boot.o main.o a.out lee.o

