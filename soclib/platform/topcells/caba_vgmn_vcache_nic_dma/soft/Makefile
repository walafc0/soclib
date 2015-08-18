CC=mipsel-unknown-elf-gcc
AS=mipsel-unknown-elf-as
LD=mipsel-unknown-elf-ld
DU=mipsel-unknown-elf-objdump

SOCLIB  = /users/outil/soc/soclib/soclib

OBJS    = reset.o \
          boot_init.o 

CFLAGS=-Wall -ffreestanding -mno-gpopt -mips32 -g

INCLUDE = -I. \
          -I$(SOCLIB)/module/connectivity_component/vci_multi_tty/include/soclib/ \
          -I$(SOCLIB)/module/connectivity_component/vci_multi_nic/include/soclib/ \
          -I$(SOCLIB)/module/infrastructure_component/dma_infrastructure/vci_chbuf_dma/include/soclib

### compilation
boot.elf: $(OBJS) boot.ld
	$(LD) -o $@ -T boot.ld $(OBJS)
	$(DU) -D $@ > $@.txt

boot_init.o: boot_init.c
	$(CC) $(INCLUDE) $(CFLAGS)  -c -o $@ $<

reset.o: reset.S
	$(CC) $(INCLUDE) $(CFLAGS)  -c -o $@ $<

### clean 
clean:
	rm -f *.o *.elf *.txt core *~	
