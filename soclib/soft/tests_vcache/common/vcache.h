/* cop2/vcache definitions */
#define VC_PTPR		$0 /* set Page Table Pointer Register */
#define VC_TLB_EN	$1 /* set Data & Inst TLBs Mode Register */
#define	  VC_TLB_EN_ITLB	0x8 /* enable instruction TLB */
#define	  VC_TLB_EN_DTLB	0x4 /* enable data TLB */
#define	  VC_TLB_EN_ICACHE	0x2 /* enable instruction cache */
#define	  VC_TLB_EN_DCACHE	0x1 /* enable data cache */
#define VC_ICACHE_FLUSH $2 /* Instruction Cache flush */
#define VC_DCACHE_FLUSH $3 /* Data Cache flush */
#define VC_ITLB_INVAL	$4 /* Instruction TLB line invalidate */
#define VC_DTLB_INVAL	$5 /* Data TLB line invalidate */
#define VC_ICACHE_INVAL $6 /* Instruction Cache line invalidate */
#define VC_DCACHE_INVAL $7 /* Data Cache line invalidate */
#define VC_ICACHE_PREFETCH $8 /* Instruction Cache line prefetch */
#define VC_DCACHE_PREFETCH $9 /* Data Cache line prefetch */
#define VC_SYNC		$10 /* Complete pending writes */
#define VC_IERR_TYPE	$11 /* Instruction Exception type Register */
#define VC_DERR_TYPE	$12 /* Data Exception type Register */
#define VC_DATA_LO	$17 /* misc register low */
#define VC_DATA_HI	$18 /* misc register hight */
#define VC_ICACHE_INVAL_PA $19 /* misc register hight */
#define VC_DCACHE_INVAL_PA $20 /* misc register hight */
#define	  VC_ERR_PT1_UNMAPPED		0x001 /* Page fault on Table1 (invalid PTE) */
#define	  VC_ERR_PT2_UNMAPPED		0x002 /* Page fault on Table 2 (invalid PTE) */
#define	  VC_ERR_PRIVILEGE_VIOLATION	0x004 /* Protected access in user mode *
#define	  VC_ERR_WRITE_VIOLATION	0x008 /* Write access to a non write page */
#define	  VC_ERR_EXEC_VIOLATION		0x010 /* Exec access to a non exec page
*/
#define	  VC_ERR_UNDEFINED_XTN		0x020 /* Undefined external access addre
ss */
#define	  VC_ERR_PT1_ILLEGAL_ACCESS	0x040 /* Bus Error in Table1 access */
#define	  VC_ERR_PT2_ILLEGAL_ACCESS	0x080 /* Bus Error in Table2 access */
#define	  VC_ERR_CACHE_ILLEGAL_ACCESS	0x100 /* Bus Error during the cache access */
#define VC_I_BAD_VADDR	$13 /* Instruction Bad Virtual Address Register */
#define VC_D_BAD_VADDR	$14 /* Data Bad Virtual Address Register */

/* vcache page level 1 format */
#define PTE1_V  (1 << 31)       /* entry valid */
#define PTE1_T  (1 << 30)       /* 0 == entry is a PTE1 (maps a 2M page) */
#define PTE1_L  (1 << 29)       /* accessed by local CPU */
#define PTE1_R  (1 << 28)       /* accessed by remote CPU */
#define PTE1_C  (1 << 27)       /* cachable */
#define PTE1_W  (1 << 26)       /* writable */
#define PTE1_X  (1 << 25)       /* executable */
#define PTE1_U  (1 << 24)       /* user-accessible */
#define PTE1_G  (1 << 23)       /* global */
#define PTE1_D  (1 << 22)       /* dirty */
/* vcache page level 2 format: same as level 1, PTE1_T  */
#define PTE2_V  (1 << 31)       /* entry valid */
				/* reserved, 0 */
#define PTE2_L  (1 << 29)       /* accessed by local CPU */
#define PTE2_R  (1 << 28)       /* accessed by remote CPU */
#define PTE2_C  (1 << 27)       /* cachable */
#define PTE2_W  (1 << 26)       /* writable */
#define PTE2_X  (1 << 25)       /* executable */
#define PTE2_U  (1 << 24)       /* user-accessible */
#define PTE2_G  (1 << 23)       /* global */
#define PTE2_D  (1 << 22)       /* dirty */
#define PTE2_os 0xff		/* OS-reserved bits */

#define PTE2_SHIFT 12
#define PTE2_MASK 0x0001ff000
#define VADDR_TO_PTE2I(va) (((va) & PTE2_MASK) >> PTE2_SHIFT)
