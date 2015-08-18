/* multi-tty definitions */
#define TTY_BASE 0xd0200000
#define TTY_WRITE  0
#define TTY_STATUS 1
#define TTY_READ 2

/* vci_exit definitions */
#define EXIT_BASE 0xe0000000

#define BOOT_ADDRESS 0xbfc00000
#define EXCEP_ADDRESS 0xbfc00380

/* vci dma */
#define DMA_BASE 0xe8000000
#define DMA_SRC  0
#define DMA_DST  4
#define DMA_LEN  8

/* cop0 definitions */
#define COP_0_BADVADDR	$8
#define COP0_STATUS	$12
#define COP0_CAUSE	$13
#define COP0_EXPC	$14

/* easy print macros */
#define PRINT(str) \
        la      a0, str;\
	jal     print; \
	nop

#define PRINTX \
	jal     printx;\
	nop

#define PUTCHAR(c) li a0, c; sb a0, 0(k0)

#define EXIT(a) \
	li a0, a; \
	sw a0, 4(k1); \
	1:      j 1b; \
	nop

/* some magic numers to test */
#define MAGIC1	0xdead
#define MAGIC2	0xbeef
#define MAGIC3	0x900d
#define MAGIC4	0xf00d

