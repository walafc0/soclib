INCLUDE seg.ld

/*
 * Grouping sections into segments
 */

SECTIONS
{
    . = seg_code_base;
    seg_code : {
        *(.text)
    }
    . = seg_data_base;
    seg_data : {
        *(.ctors)
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
}

