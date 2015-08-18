INCLUDE seg.ld

/*
 * Grouping sections into segments
 */

SECTIONS
{
    . = seg_reset_base;
    seg_reset : {
        *(.reset)
    }
    . = seg_kcode_base;
    seg_kcode : {
        *(.giet)
        *(.text)
    }
    . = seg_kdata_base;
    seg_kdata : {
        *(.rodata)
        /* . = ALIGN(4); */
        *(.rodata.*)
        /* . = ALIGN(4); */
        *(.data)
        /* . = ALIGN(4); */
        *(.lit8)
        *(.lit4)
        *(.sdata)
        /* . = ALIGN(4); */
        *(.bss)
        *(COMMON)
        *(.sbss)
        *(.scommon)
    }
    . = seg_kunc_base;
    seg_kunc : {
        *(.unckdata)
    }
}

