SECTIONS {
    . = 0x80000000;            /* Starting address */
    .text : { *(.text) }        /* Code */
    .rodata : { *(.rodata) }    /* Read-only data */
    .data : { *(.data) }        /* Initialized data */
    .bss : { *(.bss) }          /* Uninitialized data */
}