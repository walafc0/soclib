#define CTRL_REG_BASE 0xffff0000
#define CTRL_ADDR(offset) (CTRL_REG_BASE+(offset))

/* These represent the control registers */
#define CTRL_REG_START ((0xffffffff-CTRL_REG_SIZE))
#define CTRL_REG_SIZE 0xffff
#define PERIPH_START 0x1f100000
#define PERIPH_END   0x1f300000


/* The Program Status Word. */
#define PSW		CTRL_ADDR(0xfff8)

/* Saved PSW, written by hardware on exception. */
#define SAVED_PSW	CTRL_ADDR(0xfff0)

/* Saved Program Counter, written by hardware on exception. */
#define SAVED_PC	CTRL_ADDR(0xffe8)

/* The address of the exception handler code. */
#define HANDLER_PC 	CTRL_ADDR(0xffe0)

/* A one hot vector of trap (exception/interrupt) types
   indicating the cause of the last trap. Written by the hardware on a trap. */
#define EXCAUSE 	CTRL_ADDR(0xffd8)

/* This will be the data effective address in the case of 
    either a DPU, CREG, DBREAK, or MISALIGNED_TRAP exception. 
    For other exception types this register will be zero. */
#define EXADDRESS 	CTRL_ADDR(0xffd0)

/* Saved PSW, written by hardware on exception. */
#define SAVED_SAVED_PSW	CTRL_ADDR(0xffc0)

/* Saved Program Counter, written by hardware on exception. */
#define SAVED_SAVED_PC	CTRL_ADDR(0xffb8)

/* Exception cause as an integer, indicating the cause of the last trap.*/
#define EXCAUSENO 	CTRL_ADDR(0xff88)

/* Global machine state register. Controls cache locking.*/
#define STATE1 		CTRL_ADDR(0xfe00)

/* The version number of the core */
#define VERSION 	CTRL_ADDR(0xffc8)

/* Base address of peripheral registers. The top 12 bits of this register
 * are wired to the peripheral base input pins. */
#define PERIPHERAL_BASE CTRL_ADDR(0xffb0)

/* Scratch register reserved for use by supervisor software.*/
#define SCRATCH1 	CTRL_ADDR(0xffa8)
#define SCRATCH2 	CTRL_ADDR(0xffa0)
#define SCRATCH3 	CTRL_ADDR(0xff98)
/* Scratch register reserved for use by the debug interrupt handler.*/
#define SCRATCH4 	CTRL_ADDR(0xff90)


 /* Start of TLB registers */
 /* TLB index. */
#define TLB_INDEX       CTRL_ADDR(0xff80)
#define TLB_ENTRY0      CTRL_ADDR(0xff78)
#define TLB_ENTRY1      CTRL_ADDR(0xff70)
#define TLB_ENTRY2      CTRL_ADDR(0xff68)
#define TLB_ENTRY3      CTRL_ADDR(0xff60)
 /* TLB excause. */
#define TLB_EXCAUSE     CTRL_ADDR(0xff58)
 /* TLB control. */
#define TLB_CONTROL     CTRL_ADDR(0xff50)
 /* TLB replace. */
#define TLB_REPLACE     CTRL_ADDR(0xff48)
 /* TLB asid. */
#define TLB_ASID        CTRL_ADDR(0xff40)


 /* speculative load region. */
 /* SCU base 0. */
#define SCU_BASE0       CTRL_ADDR(0xd000)
 /* SCU limit 0. */
#define SCU_LIMIT0      CTRL_ADDR(0xd008)
 /* SCU base 1. */
#define SCU_BASE1       CTRL_ADDR(0xd010)
 /* SCU limit 1. */
#define SCU_LIMIT1      CTRL_ADDR(0xd018)
 /* SCU base 2. */
#define SCU_BASE2       CTRL_ADDR(0xd020)
 /* SCU limit 2. */
#define SCU_LIMIT2      CTRL_ADDR(0xd028)
 /* SCU base 3. */
#define SCU_BASE3       CTRL_ADDR(0xd030)
 /* SCU limit 3. */
#define SCU_LIMIT3      CTRL_ADDR(0xd038)


/* Data breakpoint lower address.*/
#define DBREAK_LOWER	CTRL_ADDR(0xfdd0)
#define DBREAK_UPPER	CTRL_ADDR(0xfdc8)
#define DBREAK_CONTROL  CTRL_ADDR(0xfdc0)

/* Instruction breakpoint lower address.*/
#define IBREAK_LOWER	CTRL_ADDR(0xfe80)
#define IBREAK_UPPER	CTRL_ADDR(0xfe78)
#define IBREAK_CONTROL  CTRL_ADDR(0xfe70)


/* Performance monitoring control.*/
#define PM_CR		CTRL_ADDR(0xf800)
/* Performance monitor counter x value.*/
#define PM_CNT0		CTRL_ADDR(0xf808)
#define PM_CNT1		CTRL_ADDR(0xf810)
#define PM_CNT2  	CTRL_ADDR(0xf818)
#define PM_CNT3  	CTRL_ADDR(0xf820)
/*Performance monitor core cycle counter.*/
#define PM_PCLK  	CTRL_ADDR(0xf828)

 /* port */ 
/* Port SDI0 : data, ready, control, count, timeout */ 
#define SDI0_DATA 	CTRL_ADDR(0xe000) 
#define SDI0_READY 	CTRL_ADDR(0xe008) 
#define SDI0_CONTROL 	CTRL_ADDR(0xe010) 
#define SDI0_COUNT 	CTRL_ADDR(0xe018)
#define SDI0_TIMEOUT 	CTRL_ADDR(0xe020) 
/* Port SDI1 : data, ready, control, count, timeout */ 
#define SDI1_DATA 	CTRL_ADDR(0xe400) 
#define SDI1_READY 	CTRL_ADDR(0xe408) 
#define SDI1_CONTROL 	CTRL_ADDR(0xe410) 
#define SDI1_COUNT 	CTRL_ADDR(0xe418)
#define SDI1_TIMEOUT 	CTRL_ADDR(0xe420) 
/* Port SDI2 : data, ready, control, count, timeout */ 
#define SDI2_DATA 	CTRL_ADDR(0xe800) 
#define SDI2_READY 	CTRL_ADDR(0xe808) 
#define SDI2_CONTROL 	CTRL_ADDR(0xe810) 
#define SDI2_COUNT 	CTRL_ADDR(0xe818)
#define SDI2_TIMEOUT 	CTRL_ADDR(0xe820) 
/* Port SDI3 : data, ready, control, count, timeout */ 
#define SDI3_DATA 	CTRL_ADDR(0xec00) 
#define SDI3_READY 	CTRL_ADDR(0xec08) 
#define SDI3_CONTROL 	CTRL_ADDR(0xec10) 
#define SDI3_COUNT 	CTRL_ADDR(0xec18)
#define SDI3_TIMEOUT 	CTRL_ADDR(0xec20) 






#if 0
#define NUM_UTLB_ENTRIES 64
#define KERNEL_UTLB_ENTRY 63
#define FIRST_FIXED_UTLB_ENTRY KERNEL_UTLB_ENTRY

/************ PSW bit definitions ************/
/* When 1 the core is in user mode, otherwise supervisor mode. */
#define PSW_USER_MODE			(1<< 0)
/* When 1 external interrupts are enabled. */
#define PSW_INT_ENABLE			(1<< 1)
/* When 1 enables exceptions on speculative load misalignment errors. */
#define PSW_SPECLOAD_MALIGNTRAP_EN	(1<< 4)
/* When 1 data breakpoints are enabled. */
#define PSW_DBREAK_ENABLE		(1<< 8)
/* When 1 instruction breakpoints are enabled. */
#define PSW_IBREAK_ENABLE		(1<< 9)
/* When 1 address translation is enabled. */
#define PSW_TLB_ENABLE			(1<<2)
/* When 1 prefetches can cause TLB_NO_MAPPING faults.
   When 0 prefetches that would cause a TLB_NO_MAPPING fault are discarded. */
#define PSW_PFT_NO_MAPPING_EN		(1<<3)
/* When 1 the core is in debug mode. */
#define PSW_DEBUG_MODE			(1<<12)
/* When 1 speculative loads can cause TLB_NO_MAPPING faults.
   When 0 speculative loads that would cause a TLB_NO_MAPPING fault return 0. */
#define PSW_SPECLOAD_NO_MAPPING_EN	(1<<13)
/* When 1 speculative loads can cause TLB_PROT_VIOLATION faults.
   When 0 speculative loads that would cause a TLB_PROT_VIOLATION fault return 0.
*/
#define PSW_SPECLOAD_TRAP_EN		(1<<14)

/* These are the default things we always stick in the PSW, separated out for
 * user and kernel. Done like this for uclinux, which doesn't appreciate 
 * having the TLB turned on!
 */
#ifdef CONFIG_MMU
#define PSW_KERNEL_NOSPEC (PSW_TLB_ENABLE|PSW_SPECLOAD_MALIGNTRAP_EN)
#define PSW_KERNEL_DEFAULT (PSW_TLB_ENABLE|PSW_SPECLOAD_MALIGNTRAP_EN|PSW_PFT_NO_MAPPING_EN|PSW_SPECLOAD_NO_MAPPING_EN)
#define PSW_USER_DEFAULT (PSW_USER_MODE|PSW_KERNEL_DEFAULT)
#else
#define PSW_USER_DEFAULT (PSW_USER_MODE)
#define PSW_KERNEL_DEFAULT (0)
#define PSW_KERNEL_NOSPEC (0)
#endif


/* TLB bit definitions */
#define TLB_ENTRYHI_ASID_MASK		0xff
 /* Page shared by multiple processes (ASIDs). */
#define TLB_ENTRYHI_SHARED		(1<< 8)
 /* Disabled */
#define TLB_ENTRYHI_SIZE_DISABLED	(0<< 9)
 /* 8Kb */
#define TLB_ENTRYHI_SIZE_8K		(1<< 9)
 /* 4MB */
#define TLB_ENTRYHI_SIZE_4M		(2<< 9)
 /* 256MB */
#define TLB_ENTRYHI_SIZE_256M		(3<< 9)
/* Page is dirty. When this bit is 0 write accesses to this page (when write permission is allowed) cause a TLB_WRITE_TO_CLEAN exception. When this bit is 1 writes to this page (when write permission is allowed) are permitted. */
#define TLB_ENTRYHI_DIRTY		(1<<11)
#define TLB_ENTRYHI_VADDR_MASK		0xffffe000

 /* Non cached */
#define TLB_ENTRYLO_POLICY_UNCACHED	(0<< 6)
 /* Cached */
#define TLB_ENTRYLO_POLICY_CACHED	(1<< 6)
 /* Write combining  */
#define TLB_ENTRYLO_POLICY_WCUNCACHED	(2<< 6)
 /* Reserved */
#define TLB_ENTRYLO_POLICY_RESERVED3	(3<< 6)
#define TLB_ENTRYLO_PROT_SUPER_X	(1<< 0)
#define TLB_ENTRYLO_PROT_SUPER_R	(2<< 0)
#define TLB_ENTRYLO_PROT_SUPER_W	(4<< 0)
#define TLB_ENTRYLO_PROT_SUPER_RWX	\
	(TLB_ENTRYLO_PROT_SUPER_X | TLB_ENTRYLO_PROT_SUPER_R | TLB_ENTRYLO_PROT_SUPER_W)
#define TLB_ENTRYLO_PROT_USER_X		(1<< 3)
#define TLB_ENTRYLO_PROT_USER_R		(2<< 3)
#define TLB_ENTRYLO_PROT_USER_W		(4<< 3)
#define TLB_ENTRYLO_PROT_USER_RWX	\
	(TLB_ENTRYLO_PROT_USER_X | TLB_ENTRYLO_PROT_USER_R | TLB_ENTRYLO_PROT_USER_W)
#define TLB_ENTRYLO_PADDR_SHIFT		4

#if 0
#define PAGE_SHIFT_256M		28
#define PAGE_SIZE_256M		(1<<PAGE_SHIFT_256M)
#define PAGE_MASK_256M		(~(PAGE_SIZE_256M-1))
#endif


/* TLB_EXCAUSE bit defintions */
#define TLB_EXCAUSE_INDEX	0
#define TLB_EXCAUSE_CAUSE_MASK			(3<<16)
#define TLB_EXCAUSE_CAUSE_NO_MAPPING		(0<<16)
#define TLB_EXCAUSE_CAUSE_PROT_VIOLATION	(1<<16)
#define TLB_EXCAUSE_CAUSE_WRITE_TO_CLEAN	(2<<16)
#define TLB_EXCAUSE_CAUSE_MULTI_MAPPING		(3<<16)
#define TLB_EXCAUSE_SPEC	(1<<18)
#define TLB_EXCAUSE_WRITE	(1<<19)
#define TLB_EXCAUSE_IN_UTLB	(1<<20)

/* TLB_REPLACE masks */
#define TLB_REPLACE_REPLACE_MASK	0x3f
#define TLB_REPLACE_LIMIT_SHIFT		16

/* TLB_CONTROL masks */
#define TLB_CONTROL_ITLB_FLUSH		1

/* Exception handler defines */





/* EXCAUSE bit definitions */
#define EXCAUSE_STBUS_IC_ERROR	(1<<0)	// The Instruction Cache caused a bus error.
#define EXCAUSE_STBUS_DC_ERROR	(1<<1)	// The Data Cache caused a bus error.
#define EXCAUSE_EXTERN_INT	(1<<2)	// There was an external interrupt.
#define EXCAUSE_IBREAK		(1<<3)	// An instruction address breakpoint has occured.
#define EXCAUSE_ITLB		(1<<4)	// An I side TLB exception has occured.
#define EXCAUSE_SBREAK		(1<<5)	// A software breakpoint was found.
#define EXCAUSE_ILL_INST	(1<<6)	// The bundle could not be decoded into legal sequence of operations or a privileged operation is being issued in user mode.
#define EXCAUSE_SYSCALL		(1<<7)	// System call.
#define EXCAUSE_DBREAK		(1<<8)	// The DPU has triggered a breakpoint on a data address.
#define EXCAUSE_MISALIGNED_TRAP		(1<<9)	// The address is misaligned and misaligned accesses are not supported.
#define EXCAUSE_CREG_NO_MAPPING		(1<<10)	// The load or store address was in control register space, but no control register exists at that exact address.
#define EXCAUSE_CREG_ACCESS_VIOLATION	(1<<11)	// A store to a control register was attempted whilst in user mode.
#define EXCAUSE_DTLB			(1<<12)	// An D side TLB exception has occured.
#define EXCAUSE_SDI_TIMEOUT		(1<<13)	// One of the SDI interfaces timed out while being accessed.

#endif


