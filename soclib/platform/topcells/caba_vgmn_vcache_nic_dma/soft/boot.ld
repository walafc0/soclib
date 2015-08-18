
/****************************************************************************/
/* Definition of the base addresses for all vsegs used by the GIET_VM       */
/****************************************************************************/

pseg_rom_base    = 0xBFC00000;	 /* ROM device */
pseg_ram_base    = 0x00000000;	 /* RAM device */
pseg_tty_base    = 0x90000000;   /* TTY device */
pseg_timer_base  = 0x91000000;   /* Timer device */
pseg_ioc_base    = 0x92000000;   /* Block device */
pseg_dma_base    = 0x93000000;   /* DMA device */
pseg_gcd_base    = 0x95000000;   /* GCD device */
pseg_fb_base     = 0x96000000;   /* FrameBuffer device */
pseg_nic_base    = 0x97000000;   /* NIC device */
pseg_icu_base    = 0x9F000000;   /* ICU device */

/****************************************************************************/
/* Grouping sections into virtual segment for boot code and data            */
/****************************************************************************/

SECTIONS
{
    . = pseg_rom_base;
    seg_boot : 
    {
        *(.boot)
        *(.text)

        *(.rodata)
        *(.rodata.*)
        *(.data)
        *(.lit8)
        *(.lit4)
        *(.sdata)
        *(.bss)
        *(COMMON)
        *(.sbss)
        *(.scommon)
    }
}

