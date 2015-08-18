/*
 * Definition of the base address for all segments.
 * The peripherals base addresses are referenced by the software drivers and
 * must be defined, even if the peripherals are not present in the architecture
 */

seg_reset_base  = 0xBFC00000;	/* boot code */

seg_kcode_base  = 0x80000000;	/* ststem code */
seg_kdata_base  = 0x81000000;	/* system global data */
seg_kunc_base   = 0x82000000;	/* system uncacheable data */ 

seg_code_base   = 0x00400000;	/* user code */
seg_data_base   = 0x10000000;	/* user global data */
seg_stack_base  = 0x20000000;	/* user stack */

seg_tty_base    = 0x90000000;   /* TTY device */
seg_timer_base  = 0x91000000;   /* Timer device */
seg_ioc_base    = 0x92000000;   /* Block device */
seg_dma_base    = 0x93000000;   /* DMA device */
seg_gcd_base    = 0x95000000;   /* GCD device */
seg_fb_base     = 0x96000000;   /* FrameBuffer device */
seg_icu_base    = 0x9F000000;   /* ICU device */

