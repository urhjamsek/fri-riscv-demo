/*=============================================================================
 * link.ld -- everything in the 128 KB BRAM at 0x8000_0000.
 *
 *   text / rodata / data  from the bottom
 *   bss                   after them
 *   heap                  FreeRTOS heap_4 array, in .bss
 *   stack                 grows down from the top; only used before the
 *                         scheduler starts (tasks have their own, from heap)
 *===========================================================================*/
OUTPUT_ARCH( "riscv" )
ENTRY( _start )

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128K
}

__stack_size = 2K;

SECTIONS
{
    .text : {
        KEEP(*(.text.init))
        *(.text.freertos_risc_v_trap_handler)   /* keep the 256-byte-aligned handler early */
        *(.text .text.*)
    } > RAM

    .rodata : { *(.rodata .rodata.* .srodata .srodata.*) } > RAM

    .data : ALIGN(8) {
        __global_pointer$ = . + 0x800;
        *(.data .data.* .sdata .sdata.*)
    } > RAM

    .bss : ALIGN(8) {
        __bss_start = .;
        *(.bss .bss.* .sbss .sbss.* COMMON)
        . = ALIGN(8);
        __bss_end = .;
    } > RAM

    . = ALIGN(16);
    __heap_start = .;
    __stack_top = ORIGIN(RAM) + LENGTH(RAM);
    __stack_bottom = __stack_top - __stack_size;
    ASSERT(__heap_start < __stack_bottom, "image does not fit in BRAM")

    /DISCARD/ : { *(.eh_frame .eh_frame_hdr .note.* .comment) }
}
